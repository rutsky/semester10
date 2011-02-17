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

#ifndef FREQ_TABLE_HPP_
#define FREQ_TABLE_HPP_

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <stdexcept>
#include <iomanip>

#include <boost/assert.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>

class FreqTable : public std::map<char, double>
{
};

// TODO: Maybe work on character iterator?
template< class CharT, class Traits >
inline
FreqTable calcFreqTable( std::basic_istream<CharT, Traits> &is )
{
  FreqTable table;
  double totalCount(0);

  while (true)
  {
    char ch;
    is >> ch;
    if (!is)
      break;

    if (table.find(ch) == table.end())
      table[ch] = 0;

    ++table[ch];
    ++totalCount;
  }

  BOOST_FOREACH(FreqTable::value_type &pair, table)
  {
    pair.second /= totalCount;
  }

  return table;
}

template< class CharT, class Traits >
inline
std::basic_istream<CharT, Traits> &
    operator >> ( std::basic_istream<CharT, Traits> &is, FreqTable &table )
{
  table.clear();

  double freqSum(0);

  std::string str;
  while (std::getline(is, str))
  {
    if (str.length() == 0 || str[0] == '#')
      continue;

    std::istringstream istr(str);

    char ch;
    double freq(-1);
    istr >> ch >> freq;
    if (freq <= 0)
    {
      throw std::runtime_error(
          (boost::format("Invalid frequency string: %1%") % str).str());
    }

    table[ch] = freq;
    freqSum += freq;
  }

  if (!table.empty() && freqSum < 0.999)
  {
    throw std::runtime_error(
        (boost::format(
            "Sum of frequencies not even close to 1.0: %1%") % freqSum).str());
  }

  return is;
}

template< class CharT, class Traits >
inline
std::basic_ostream<CharT, Traits> &
    operator << ( std::basic_ostream<CharT, Traits> &os, 
                  FreqTable const &table )
{
  BOOST_FOREACH(FreqTable::value_type const &pair, table)
  {
    os << pair.first << " " << std::setprecision(16) << pair.second << "\n";
  }

  return os;
}

#endif // FREQ_TABLE_HPP_

// vim: set ts=2 sw=2 et:
