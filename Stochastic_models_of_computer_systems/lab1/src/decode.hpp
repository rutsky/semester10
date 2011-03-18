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

#include "freq_table2.hpp"

template< class CharInIt >
inline
void decode( freq1_map_t const &fm1, freq2_map_t const &fm2, 
             double gamma, double alpha, double zeta,
             CharInIt first, CharInIt beyond )
{
  std::vector<char> input(first, beyond);
}

#endif // DECODE_HPP_

// vim: set ts=2 sw=2 et:
