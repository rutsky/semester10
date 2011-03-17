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

#include <boost/array.hpp>

/*****
 * Types definition
 *****/

template< class SymbType, class ScalarType, size_t n >
struct freq_vec_t : std::vector<std::pair<boost::array<SymbType, n>, ScalarType> >
{
};

template< class SymbType, class ScalarType, size_t n >
struct freq_map_t : std::map<boost::array<SymbType, n>, ScalarType>
{
};

typedef freq_vec_t<char, double, 1> freq1_vec_t;
typedef freq_vec_t<char, double, 2> freq2_vec_t;
typedef freq_map_t<char, double, 1> freq1_map_t;
typedef freq_map_t<char, double, 2> freq2_map_t;

#endif // FREQ_TABLE2_HPP_

// vim: set ts=2 sw=2 et:
