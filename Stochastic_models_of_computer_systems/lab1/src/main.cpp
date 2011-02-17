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

#include <iostream>
#include <vector>

#include <boost/optional.hpp>

#include "chi_square.h"
#include "freq_table.hpp"

typedef boost::optional<biection_t const> decode_result_t;

decode_result_t decode( FreqTable const &trueTable, FreqTable const &table, 
                        double alpha )
{
  FreqTable trueTableCopy(trueTable);
  FreqTable tableCopy(table);

  updateWithKeys(trueTableCopy, tableCopy);

  std::cerr << trueTable << "\n"; // DEBUG
  std::cerr << table << "\n"; // DEBUG

  VectorFreqTable trueTableVec = toVector(trueTableCopy);
  VectorFreqTable tableVec = toVector(tableCopy);

  sort(trueTableVec);
  sort(tableVec);

  double const chi2 = chiSquareCharact(trueTableVec, tableVec);
  std::cout << chi2 << "\n"; // DEBUG
  double const chi2Cr = chiSquareCritical(alpha, trueTableVec.size());
  std::cout << chi2Cr << "\n"; // DEBUG
  if (chi2 < chi2Cr)
  {
    biection_t const biection = buildBiection(trueTableVec, tableVec);
    return decode_result_t(biection);
  }
  else
    return decode_result_t();
}

int main( int argc, char *argv[] )
{
  if (argc != 2)
  {
    std::cout << 
        "Usage:\n" <<
        "  " << argv[0] << " true_freq_table < encoded_text\n";
    return 0;
  }

  FreqTable trueTable;
  {
    std::fstream trueTableFS(argv[1]);
    if (!trueTableFS)
    {
      perror(argv[1]);
      return 1;
    }
    trueTableFS >> trueTable;
  }

  std::string input(
      std::istream_iterator<char>(std::cin),
      std::istream_iterator<char>());
  std::vector<std::string> chs;
  for (size_t i = 0; i < input.size(); ++i)
  {
    std::ostringstream ostr;
    ostr << input[i];
    chs.push_back(ostr.str());
  }
  FreqTable table = calcFreqTable(chs.begin(), chs.end());

  double const alpha = 0.9;
  decode_result_t res = decode(trueTable, table, alpha);
  if (res)
  {
    biection_t const &biection = *res;
    BOOST_FOREACH(char ch, input)
    {
      biection_t::const_iterator it = biection.find(ch);
      BOOST_ASSERT(it != biection.end());
      std::cout << it->second;
    }
  }
  else
  {
    std::cerr << 
      "Input message is not encoded English text "
      "with statistical significance " << alpha << "\n";
  }
}

// vim: set ts=2 sw=2 et:
