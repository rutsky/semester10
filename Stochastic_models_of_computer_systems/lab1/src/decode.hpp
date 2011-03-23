/*
 *  Copyright (C) 2011  Vladimir Rutsky <altsysrq@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DECODE_HPP_
#define DECODE_HPP_

#include <vector>
#include <set>
#include <fstream>
#include <iostream>

#include <boost/foreach.hpp>
#include <boost/assert.hpp>
#include <boost/math/distributions/binomial.hpp>

#include "freq_table.hpp"

// TODO: Optimize.
// TODO: Use visitor pattern.
template< class CurrentCharIt, class BijectionOutIt >
inline
size_t find_bijections( std::map<char, std::vector<char> > const &theorToEmp,  
                        CurrentCharIt first, CurrentCharIt beyond,
                        std::vector<int> &currentBijection,
                        std::vector<int> &usedChars,
                        size_t numFoundBijections,
                        size_t maxNumberOfBijections,
                        BijectionOutIt outIt )
{
  typedef std::vector<char> possible_symb_t;
  typedef std::map<char, possible_symb_t> theor_to_emp_symbs_t;

  if (first == beyond)
  {
    //*outIt++ = currentBijection;
    
    if ((numFoundBijections + 1) % 1000000 == 0)
      std::cout << "Already found " << numFoundBijections + 1 << 
          " bijections..." << std::endl;
    
    // DEBUG
    //static int cc(0);
    //std::cout << cc << "\n";
    //if (cc == 100000000)
    //{
    //  std::cout << "limit exceeded!\n";
    //  return false;
    //}
    //++cc;

    return 1;
  }
  else
  {
    char ch = *first++;

    BOOST_ASSERT(!currentBijection.at(ch));
    BOOST_ASSERT(theorToEmp.find(ch) != theorToEmp.end());

    size_t foundOnThisStep(0);
    BOOST_FOREACH(char empCh, theorToEmp.find(ch)->second)
    {
      if (!usedChars.at((unsigned char)empCh))
      {
        currentBijection.at(ch) = empCh;
        usedChars.at((unsigned char)empCh) = 1;
        
        foundOnThisStep += find_bijections(theorToEmp, first, beyond, 
            currentBijection, usedChars, 
            numFoundBijections + foundOnThisStep, maxNumberOfBijections, 
            outIt);

        usedChars.at((unsigned char)empCh) = 0;
        currentBijection.at(ch) = 0;
        
        BOOST_ASSERT(
            numFoundBijections + foundOnThisStep <= maxNumberOfBijections);
        if (numFoundBijections + foundOnThisStep >= maxNumberOfBijections)
          return foundOnThisStep;
      }
    }
    return foundOnThisStep;
  }
}

template< class CharInIt >
inline
int decode( freq1_map_t const &fm1, freq2_map_t const &fm2, 
             double gamma, double alpha, double zeta,
             CharInIt first, CharInIt beyond )
{
  char const 
      *freq1FileName = "frequency1.dat",
      *freq2FileName = "frequency2.dat",
      *bijectionsFileName = "bijections.dat";
  size_t const maxNumberOfBijections(1e8);

  std::cout << 
      "Confidence level of reducing number of bijections confidence "
      "intervals, gamma = " << gamma << std::endl <<
      "Significance level for Pearson's Chi-square test, alpha = " << 
          alpha << std::endl <<
      "Minimal acceptable probability of observed Markov chain, zeta = " <<
          zeta << std::endl; 

  // Read input.
  std::vector<char> input(first, beyond);

  // Construct alphabet.
  std::set<char> chars;
  alphabet(fm1, std::inserter(chars, chars.begin()));

  // Calculate empiric frequencies.
  freq1_map_t efm1(fm1);
  calculate_frequency(input.begin(), input.end(), efm1);

  freq2_map_t efm2(fm2);
  calculate_frequency(input.begin(), input.end(), efm2);
  
  // Write empiric frequencies to files.
  {
    std::ofstream os(freq1FileName);
    
    if (!os)
    {
      perror(freq1FileName);
      return 1;
    }
    else
    {
      os << efm1;
    }
  }
  {
    std::ofstream os(freq2FileName);
    
    if (!os)
    {
      perror(freq2FileName);
      return 1;
    }
    else
    {
      os << efm2;
    }
  }
  
  // Calculate confidence intervals for theoretic frequencies.
  typedef std::pair<double, double> interval_t;
  typedef std::map<char, interval_t> confidence_intervals_map_t;
  confidence_intervals_map_t confInt;

  BOOST_FOREACH(freq1_map_t::value_type const &pair, fm1)
  {
    typedef boost::math::binomial_distribution<double> binomial_distr_t;
    double const lo = 
        binomial_distr_t::find_lower_bound_on_p(
            input.size(), input.size() * pair.second, (1.0 - gamma) / 2.0);
    double const hi = 
        binomial_distr_t::find_upper_bound_on_p(
            input.size(), input.size() * pair.second,  (1.0 - gamma) / 2.0);

    confInt[pair.first[0]] = interval_t(lo, hi);
  }
  
  // Find all empiric frequencies that match each confidence interval.
  typedef std::vector<char> possible_symb_t;
  typedef std::map<char, possible_symb_t> theor_to_emp_symbs_t;
  theor_to_emp_symbs_t theorToEmp;

  // TODO: Optimize.
  size_t totalMatches(0);
  BOOST_FOREACH(freq1_map_t::value_type const &theorPair, fm1)
  {
    possible_symb_t &possible = theorToEmp[theorPair.first[0]];

    interval_t const &interval = confInt[theorPair.first[0]];
    BOOST_FOREACH(freq1_map_t::value_type const &empPair, efm1)
    {
      if (empPair.second >= interval.first && empPair.second <= interval.second)
      {
        // Found empirical symbol whom frequency lies in confidence interval 
        // of current theoretical symbol.
        possible.push_back(empPair.first[0]);
        ++totalMatches;
      }
    }
  }

  // Output all matches.
  // TODO: Order by intervals.
  // TODO: Also output is identity bijection included.
  size_t worstNumOfBijections(1);
  std::cout << "Found " << totalMatches << " matches of empirical frequencies "
      "with theoretical confidence intervals.\n";
  std::cout << "Symbol (confidence interval): matched symbols\n";
  bool identityBijectionIncluded(true);
  BOOST_FOREACH(theor_to_emp_symbs_t::value_type const &pair, theorToEmp)
  {
    std::cout << "'" << pair.first << "': (" << 
            std::setw(16) << std::setprecision(16) << std::fixed << 
                confInt[pair.first].first << ", " <<
            std::setw(16) << std::setprecision(16) << std::fixed <<
                confInt[pair.first].second << "): ";
    BOOST_FOREACH(char symb, pair.second)
    {
      if (symb == ' ')
        std::cout << "' ' ";
      else
        std::cout << symb << " ";
    }
    std::cout << "\n";

    worstNumOfBijections *= pair.second.size();

    identityBijectionIncluded = identityBijectionIncluded &&
      (std::find(pair.second.begin(), pair.second.end(), pair.first) != 
            pair.second.end());
  }
  std::cout << 
      "Worst number of bijections: " << worstNumOfBijections << std::endl;
  if (!identityBijectionIncluded)
  {
    std::cout << 
        "Identity bijection not included, this is definitely not good.\n"
        "Skip other checks.\n";
    return 0;
  }

  // Generate all possible bijections.
  typedef std::map<char, char> bijection_t;
  typedef std::vector<bijection_t> bijections_vec_t;
  bijections_vec_t bijections;
  //bijection_t tmpBijection;
  std::vector<int> tmpBijection(256, 0);
  std::vector<int> tmpChars(256, 0);
  find_bijections(theorToEmp, chars.begin(), chars.end(), 
      tmpBijection, tmpChars, 0, maxNumberOfBijections, 
      std::back_inserter(bijections));

  std::cout << "Found " << bijections.size() << " bijections.\n";

  // Write bijections to file.
  {
    std::ofstream os(bijectionsFileName);
    if (!os)
    {
      perror(bijectionsFileName);
      return 1;
    }
    else
    {
      BOOST_FOREACH(bijections_vec_t::value_type bijection, bijections)
      {
        BOOST_FOREACH(bijection_t::value_type const &pair, bijection)
        {
          os << pair.second;
        }
        os << "\n";
      }
    }
  }

  // Go over all possible bijections and store bijections that pass Pearson's
  // Chi-square test.
  

  // For each bijection that passed Pearson's Chi-square test output bijections
  // which probability in Markov's chain is not less than zeta.

  
  return 0;
}

#endif // DECODE_HPP_

// vim: set ts=2 sw=2 et:
