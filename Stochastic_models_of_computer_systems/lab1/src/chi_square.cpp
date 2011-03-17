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

#include "chi_square.h"

#include <cmath>
#include <stdexcept>

#include "common.hpp"

// TODO: Rename to something about quantiles.
// TODO: Use Boost implementation.
double chiSquareCritical( double alpha, size_t n )
{
  // Golberg H., Levine H. Approximate formulas for the percentage points and 
  // normalization of t and χ2 // AMS. 1945. V.17. P. 216-225.
  double d;
  if (0.5 <= alpha && alpha <= 0.999)
    d =  2.0637 * pow(log(1.0 / (1 - alpha)) - 0.16, 0.4274) - 1.5774;
  else if (0.001 <= alpha && alpha < 0.5)
    d = -2.0637 * pow(log(1.0 / alpha)       - 0.16, 0.4274) + 1.5774;
  else
    throw std::logic_error("Not implemented");

  double const A = d * sqrt(2.0);
  double const B = 2.0 / 3 * (sqr(d) - 1);
  double const C = d * (sqr(d) - 7) / (9 * sqrt(2.0));
  double const D = (6 * pow(d, 4) + 14 * sqr(d) - 32) / 405.0;
  double const E = d * (9 * pow(d, 4) + 256 * sqr(d) - 433) / (4860 * sqrt(2));

  double const result = n + A * sqrt(n) + B + C / sqrt(n) + D / n + 
      E / (n * sqrt(n));

  return result;
}

// vim: set ts=2 sw=2 et:
