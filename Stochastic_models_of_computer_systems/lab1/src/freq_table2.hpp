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

#ifndef FREQ_TABLE2_HPP_
#define FREQ_TABLE2_HPP_

#include <cstddef>
#include <vector>
#include <map>
#include <set>
#include <stdexcept>
#include <cmath>

#include <boost/array.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>

/*****
 * Types definition.
 *****/

template< class SymbT, class ScalarT, std::size_t Len >
struct freq_vec_t
  : std::vector<std::pair<boost::array<SymbT, Len>, ScalarT> >
{
  typedef SymbT   symb_t;
  typedef ScalarT scalar_t;

  typedef boost::array<symb_t, Len> chain_t;

  static int const chain_length = Len;
};

template< class SymbT, class ScalarT, std::size_t Len >
struct freq_map_t
  : std::map<boost::array<SymbT, Len>, ScalarT>
{
  typedef SymbT   symb_t;
  typedef ScalarT scalar_t;

  typedef boost::array<symb_t, Len> chain_t;

  static int const chain_length = Len;
};

typedef freq_vec_t<char, double, 1> freq1_vec_t;
typedef freq_vec_t<char, double, 2> freq2_vec_t;
typedef freq_map_t<char, double, 1> freq1_map_t;
typedef freq_map_t<char, double, 2> freq2_map_t;

/*****
 * Operations with frequency tables.
 *****/

template< class SymbT, class ScalarT, class OutT, size_t Len >
size_t alphabet( freq_map_t<SymbT, ScalarT, Len> const &fm, OutT outIt )
{
  typedef freq_map_t<SymbT, ScalarT, Len> fm_t;

  std::set<typename fm_t::symb_t> chars;

  BOOST_FOREACH(typename fm_t::value_type const &chain_pair, fm)
  {
    std::copy(chain_pair.first.begin(), chain_pair.first.end(), 
        std::inserter(chars, chars.begin()));
  }

  std::copy(chars.begin(), chars.end(), outIt);
}

template< class SymbT, class ScalarT, class OutT, size_t Len >
bool is_complete( freq_map_t<SymbT, ScalarT, Len> const &fm )
{
  typedef freq_map_t<SymbT, ScalarT, Len> fm_t;

  std::vector<typename fm_t::symb_t> tmp; // TODO: use dummy iterator
  size_t const nChars = alphabet(fm, std::back_inserter(tmp));

  // TODO: Use integral calculations.
  size_t const nReqFreq = std::pow(nChars, fm_t::chain_length);

  return fm.size() == nReqFreq;
}

/*****
 * Frequency tables I/O.
 *****/

template< class CharT, class Traits, 
          class SymbT, class ScalarT, size_t Len >
inline
std::basic_istream<CharT, Traits> &
    operator >> ( std::basic_istream<CharT, Traits> &is, 
                  freq_map_t<SymbT, ScalarT, Len> &fm )
{
  typedef freq_map_t<SymbT, ScalarT, Len> fm_t;
  typedef typename fm_t::scalar_t scalar_t;

  fm.clear();

  scalar_t freqSum(0);

  std::string chStr, freqStr;
  while (std::getline(is, chStr) && std::getline(is, freqStr))
  {
    if (chStr.length() == 0 || freqStr.length() == 0)
      continue;

    if (chStr.length() != Len)
    {
      throw std::runtime_error(
          (boost::format("Invalid chain length (%1%): %2%") % 
              chStr.length() % chStr).str());
    }

    typename fm_t::chain_t chain;
    std::copy(chStr.begin(), chStr.end(), chain.begin());

    std::istringstream istr(freqStr);
    scalar_t freq(-1);
    istr >> freq;
    if (freq <= 0)
    {
      throw std::runtime_error(
          (boost::format("Invalid frequency string: %1%") % freqStr).str());
    }

    fm[chain] = freq;
    freqSum += freq;
  }

  if (!fm.empty() && freqSum < 0.999)
  {
    throw std::runtime_error(
        (boost::format(
            "Sum of frequencies not even close to 1.0: %1%") % freqSum).str());
  }

  if (is_complete(fm))
  {
    throw std::runtime_error(
        (boost::format(
            "Not all frequencies listed")).str());
  }

  return is;
}

/*
template< class CharT, class Traits,
          class SymbT, class ScalarT, size_t Len >
inline
std::basic_ostream<CharT, Traits> &
    operator << ( std::basic_ostream<CharT, Traits> &os, 
                  freq_map_t<SymbT, ScalarT, Len> &fm )
{
  typedef freq_map_t<SymbT, ScalarT, Len> fm_t;

  VectorFreqTable vec = toVector(table);
  sort(vec);
  
  BOOST_FOREACH(VectorFreqTable::value_type const &pair, vec)
  {
    os << pair.first << "\n" << std::setprecision(16) << pair.second << "\n";
  }

  return os;
}*/

#endif // FREQ_TABLE2_HPP_

// vim: set ts=2 sw=2 et:
