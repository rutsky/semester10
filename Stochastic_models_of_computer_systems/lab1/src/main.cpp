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

#include "chi_square.h"
#include "freq_table.hpp"

bool decode( FreqTable const &trueTable, FreqTable const &table )
{
  VectorFreqTable trueTableVec = toVector(trueTable);
  VectorFreqTable tableVec = toVector(table);

  sort(trueTableVec);
  sort(tableVec);
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

  FreqTable table = calcFreqTable(std::cin);

  decode(trueTable, table);
}

// vim: set ts=2 sw=2 et:
