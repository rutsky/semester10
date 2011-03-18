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
#include <fstream>

#include <boost/program_options.hpp>

#include "freq_table.hpp"
#include "decode.hpp"

namespace po = boost::program_options;

int main( int argc, char *argv[] )
{
  std::string freq1FileName, freq2FileName;
  double gamma, alpha, zeta;

  // Parse command line.
  try
  {
    po::options_description optDesc("Allowed options");
    optDesc.add_options()
        ("help", "display help message.")
        ("1-frequency", po::value<std::string>(&freq1FileName), 
            "single characters frequencies.")
        ("2-frequency", po::value<std::string>(&freq2FileName), 
            "single characters frequencies.")
        ("gamma", po::value<double>(&gamma),
            "confidence level of reducing number of bijections confidence "
            "intervals.")
        ("alpha", po::value<double>(&alpha),
            "significance level for Pearson's Chi-square test.")
        ("zeta", po::value<double>(&zeta),
            "minimal acceptable probability of observed Markov chain.");
    po::positional_options_description posOptDesc;
    posOptDesc
        .add("1-frequency", 1)
        .add("2-frequency", 1)
        .add("gamma", 1)
        .add("alpha", 1)
        .add("zeta", 1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
        options(optDesc).positional(posOptDesc).run(), vm);
    po::notify(vm);

    bool const haveRequiredOptions = 
        vm.count("1-frequency") && 
        vm.count("2-frequency") &&
        vm.count("gamma") &&
        vm.count("alpha") &&
        vm.count("zeta");

    if (vm.count("help") || !haveRequiredOptions)
    {
      std::cout << optDesc << "\n";
      return (haveRequiredOptions ? 0 : 1);
    }
  }
  catch( std::exception &ex )
  {
    std::cerr << "Error: " << ex.what() << std::endl;
    return 1;
  }

  // Load frequency tables.
  freq1_map_t fm1;
  freq2_map_t fm2;

  try
  {
    {
      std::ifstream is(freq1FileName.c_str());

      if (!is)
      {
        perror(freq1FileName.c_str());
        return 1;
      }
      else
      {
        is >> fm1;
      }
    }

    {
      std::ifstream is(freq2FileName.c_str());

      if (!is)
      {
        perror(freq2FileName.c_str());
        return 1;
      }
      else
      {
        is >> fm2;
      }
    }
  }
  catch( std::exception &ex )
  {
    std::cerr << "Error: " << ex.what() << std::endl;
    return 1;
  }

  // Check that both table have same alphabet.
  std::set<char> alphabet1, alphabet2;
  alphabet(fm1, std::inserter(alphabet1, alphabet1.begin()));
  alphabet(fm2, std::inserter(alphabet2, alphabet2.begin()));
  if (alphabet1 != alphabet2)
  {
    std::cerr << "Error: alphabets differs in frequencies tables\n";
    return 1;
  }
  
  // Read whole input and check it's alphabet.
  std::noskipws(std::cin);
  std::vector<char> input(
      std::istream_iterator<char>(std::cin),
      std::istream_iterator<char>());
  
  std::vector<char> filteredInput;
  bool filtered(false);
  BOOST_FOREACH(char ch, input)
  {
    if (alphabet1.find(ch) != alphabet1.end())
    {
      filteredInput.push_back(ch);
    }
    else
    {
      if (!filtered)
      {
        filtered = true;
        std::cerr << 
          "Warning: found symbols outside frequency tables alphabet.\n";
      }
    }
  }

  if (filteredInput.size() < 2)
  {
    std::cerr << "Error: too small input. Need at least two characters.\n";
    return 1;
  }

  /*
  std::set<char> chars;
  std::copy(input.begin(), input.end(), std::inserter(chars, chars.begin()));
  if (!std::includes(alphabet1.begin(), alphabet1.end(), 
                     chars.begin(), chars.end()))
  {
    std::cerr << 
        "Error: input has characters not described in frequency tables.\n";
    return 1;
  }
  */

  // Start decoding process.
  decode(fm1, fm2, gamma, alpha, zeta, 
      filteredInput.begin(), filteredInput.end());
}

// vim: set ts=2 sw=2 et:
