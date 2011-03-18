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
  
  // Find all emperic frequencies that match each confidence interval.

  // Output all matches.

  
  return 0;
}

#endif // DECODE_HPP_

// vim: set ts=2 sw=2 et:
