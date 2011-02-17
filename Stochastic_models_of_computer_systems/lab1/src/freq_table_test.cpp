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

#include "freq_table.hpp"

#include <sstream>

BOOST_AUTO_TEST_SUITE(freq_table)

BOOST_AUTO_TEST_CASE(test_main)
{
  std::string testText("aabcac");
  std::istringstream istr(testText);

  FreqTable table = calcFreqTable(istr);

  BOOST_CHECK_EQUAL(table.size(), 3);
  BOOST_CHECK(table.find('a') != table.end());
  BOOST_CHECK(table.find('b') != table.end());
  BOOST_CHECK(table.find('c') != table.end());

  BOOST_CHECK_CLOSE(table['a'], 3.0 / 6, 1e-3);
  BOOST_CHECK_CLOSE(table['b'], 1.0 / 6, 1e-3);
  BOOST_CHECK_CLOSE(table['c'], 2.0 / 6, 1e-3);

  {
    std::ostringstream ostr;
    ostr << table;
    std::istringstream istr(ostr.str());
    FreqTable table2;
    istr >> table2;

    BOOST_CHECK_EQUAL(table2.size(), 3);
    BOOST_CHECK_CLOSE(table2['a'], table['a'], 1e-3);
    BOOST_CHECK_CLOSE(table2['b'], table['b'], 1e-3);
    BOOST_CHECK_CLOSE(table2['c'], table['c'], 1e-3);
  }

  VectorFreqTable vec;
  vec = toVector(table);
  BOOST_CHECK_EQUAL(vec.size(), 3);
  BOOST_CHECK_EQUAL(vec[0].first, 'a');
  BOOST_CHECK_CLOSE(vec[0].second, table['a'], 1e-3);
  BOOST_CHECK_EQUAL(vec[1].first, 'b');
  BOOST_CHECK_CLOSE(vec[1].second, table['b'], 1e-3);
  BOOST_CHECK_EQUAL(vec[2].first, 'c');
  BOOST_CHECK_CLOSE(vec[2].second, table['c'], 1e-3);

  sort(vec);

  BOOST_CHECK_EQUAL(vec[0].first, 'b');
  BOOST_CHECK_CLOSE(vec[0].second, table['b'], 1e-3);
  BOOST_CHECK_EQUAL(vec[1].first, 'c');
  BOOST_CHECK_CLOSE(vec[1].second, table['c'], 1e-3);
  BOOST_CHECK_EQUAL(vec[2].first, 'a');
  BOOST_CHECK_CLOSE(vec[2].second, table['a'], 1e-3);
}

BOOST_AUTO_TEST_SUITE_END()

// vim: set ts=2 sw=2 et:
