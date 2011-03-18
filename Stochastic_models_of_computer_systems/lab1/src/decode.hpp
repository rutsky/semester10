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

template< class CharInIt >
inline
int decode( freq1_map_t const &fm1, freq2_map_t const &fm2, 
             double gamma, double alpha, double zeta,
             CharInIt first, CharInIt beyond )
{
  char const 
      *freq1FileName = "frequency1.dat",
      *freq2FileName = "frequency2.dat";

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
  std::cout << "Found " << totalMatches << " matches of empirical frequencies "
      "with theoretical confidence intervals.\n";
  std::cout << "Symbol (confidence interval): matched symbols\n";
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
  }
  
  return 0;
}

#endif // DECODE_HPP_

// vim: set ts=2 sw=2 et:
