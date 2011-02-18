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
#include <vector>
#include <stdexcept>
#include <iomanip>

#include <boost/assert.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/bind.hpp>

#include "common.hpp"

// TODO: Rename to freq_table_t.
class FreqTable : public std::map<std::string, double>
{
};

// TODO: Rename to freq_table_t.
class VectorFreqTable : public std::vector<std::pair<std::string, double> >
{
};

void updateWithKeys( FreqTable &table1, FreqTable &table2 )
{
  // TODO: Not optimal.
  BOOST_FOREACH(FreqTable::value_type const &pair, table1)
  {
    if (table2.find(pair.first) == table2.end())
      table2[pair.first] = 0;
  }
  BOOST_FOREACH(FreqTable::value_type const &pair, table2)
  {
    if (table1.find(pair.first) == table1.end())
      table1[pair.first] = 0;
  }
}

VectorFreqTable toVector( FreqTable const &table )
{
  VectorFreqTable vec;
  std::copy(table.begin(), table.end(), std::back_inserter(vec));
  return vec;
}

void sort( VectorFreqTable &table )
{
  std::sort(table.begin(), table.end(), 
      boost::bind(&VectorFreqTable::value_type::second, _1) <
      boost::bind(&VectorFreqTable::value_type::second, _2));
}

double chiSquareCharact( VectorFreqTable const &trueVec, 
                         VectorFreqTable const &vec )
{
  BOOST_ASSERT(trueVec.size() == vec.size());
  
  size_t const n(trueVec.size());
  double ch(0);
  for (size_t i = 0; i < n; ++i)
  {
    ch += sqr(vec[i].second - trueVec[i].second) / trueVec[i].second;
  }

  return ch * n;
}

typedef std::map<char, char> biection_t;
biection_t buildBiection( VectorFreqTable const &trueVec, 
                          VectorFreqTable const &vec )
{
  BOOST_ASSERT(trueVec.size() == vec.size());

  biection_t biection;
  for (int i = vec.size() - 1; i >= 0; --i)
  {
    // TODO
    //BOOST_ASSERT(biection.find(vec[i].first[0]) == biection.end());
    BOOST_ASSERT(vec[i].first.length() == trueVec[i].first.length());
    for (size_t j = 0; j < vec[i].first.length(); ++j)
    {
      char const from = vec[i].first[j];
      char const to = trueVec[i].first[j];
      if (biection.find(from) == biection.end())
        biection[from] = to;
    }
  }

  return biection;
}

template< class CharInputIt >
inline
FreqTable calcFreqTable( CharInputIt first, CharInputIt beyond )
{
  FreqTable table;
  double totalCount(0);

  for (; first != beyond; ++first)
  {
    std::string ch = *first;

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

// TODO: Maybe work on character iterator?
// TODO: Move to separate file `freq_table_io.hpp' and other source from here
// move to freq_table.cpp.
//template< class CharT, class Traits >
//inline
//FreqTable calcFreqTable( std::basic_istream<CharT, Traits> &is )
//{
//  return calcFreqTable(
//      std::istream_iterator<char>(is),
//      std::istream_iterator<char>());
//}

template< class CharT, class Traits >
inline
std::basic_istream<CharT, Traits> &
    operator >> ( std::basic_istream<CharT, Traits> &is, FreqTable &table )
{
  table.clear();

  double freqSum(0);

  std::string chStr, freqStr;
  while (std::getline(is, chStr) && std::getline(is, freqStr))
  {
    if (chStr.length() == 0 || freqStr.length() == 0)
      continue;

    std::istringstream istr(freqStr);
    double freq(-1);
    istr >> freq;
    if (freq <= 0)
    {
      throw std::runtime_error(
          (boost::format("Invalid frequency string: %1%") % freqStr).str());
    }

    table[chStr] = freq;
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
  VectorFreqTable vec = toVector(table);
  sort(vec);
  
  BOOST_FOREACH(VectorFreqTable::value_type const &pair, vec)
  {
    os << pair.first << "\n" << std::setprecision(16) << pair.second << "\n";
  }

  return os;
}

#endif // FREQ_TABLE_HPP_

// vim: set ts=2 sw=2 et:
