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

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN

#include <boost/test/unit_test.hpp>

#include <fstream>

#include "freq_table.hpp"

BOOST_AUTO_TEST_SUITE(freq_table2)

BOOST_AUTO_TEST_CASE(test_main)
{
  freq1_vec_t f1;
  freq2_vec_t f2;

  freq1_map_t fm1;
  freq2_map_t fm2;

  {
    std::ifstream is("table/table.dat");
    BOOST_CHECK(is);

    is >> fm1;
  }

  {
    std::ifstream is("table/table2.dat");
    BOOST_CHECK(is);

    is >> fm2;
  }

  f1 = fm1;
  f2 = fm2;
}

BOOST_AUTO_TEST_SUITE_END()

// vim: set ts=2 sw=2 et:
