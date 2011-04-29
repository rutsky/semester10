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
#include <fstream>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/foreach.hpp>
#include <boost/math/distributions/normal.hpp>

namespace po = boost::program_options;

template< class T >
T sqr( T const &v )
{
  return v * v;
}

typedef std::vector<double> measurements_t;

void estimate( measurements_t const &measurements, double dt,
               size_t quiet_period, double requests_detection_alpha )
{
  BOOST_ASSERT(quiet_period >= 2);
  BOOST_ASSERT(dt >= 1e-8);
  BOOST_ASSERT(0 <= requests_detection_alpha && requests_detection_alpha <= 1);

  char const *detectedRequestsFile = "detected_requests.txt";

  // Calculate numeric derivative.
  std::vector<double> der;
  der.reserve(measurements.size());
  der.push_back(0);
  for (size_t i = 1; i < measurements.size(); ++i)
  {
    der.push_back(measurements[i] - measurements[i - 1]);
  }

  // Detect times when requests arrived.
  std::vector<size_t> T_c;
  size_t lastRequestIdx = 0;
  while (lastRequestIdx + 1 + quiet_period + 1 < measurements.size())
  {
    // Estimate \sigma^2 on quiet period (period after request in 
    // which no request arrived).
    double accum(0);
    for (size_t i = 0; i < quiet_period; ++i)
      accum += sqr(der[lastRequestIdx + 1 + i]);

    size_t n = quiet_period;

    size_t idx = lastRequestIdx + 1 + quiet_period;
    BOOST_ASSERT(idx < measurements.size());

    bool foundRequest(false);
    for (; idx < measurements.size(); 
        accum += sqr(der[idx]), ++idx, ++n)
    {
      double const sigma2 = accum / ((n - 1) * dt);

      // Construct normal distribution for currently estimated \sigma^2
      boost::math::normal_distribution<> 
          normalDistr(0, std::sqrt(sigma2 * dt));
      double const loQuantile = 
          quantile(normalDistr, 
              requests_detection_alpha / 2.0);
      double const hiQuantile = 
          quantile(complement(
              normalDistr, 
              requests_detection_alpha / 2.0));

      // DEBUG
      std::cout << idx << " ";
      std::cout << "  sigma2: " << sigma2 << 
        ", loQuantile: " << loQuantile <<
        ", hiQuantile: " << hiQuantile << 
        ", det: " << der[idx] << "\n";
      // END OF DEBUG
      
      // Check if next observing value lies in rare quantiles.
      if (der[idx] < loQuantile || der[idx] > hiQuantile)
      {
        // Rare event happened - request.
        std::cout << "***\n"; // DEBUG
        foundRequest = true;
        T_c.push_back(idx);
        lastRequestIdx = idx;
        break;
      }
    }

    if (!foundRequest)
      break;
  }

  // Write detected requests in file.
  {
    std::ofstream ofs(detectedRequestsFile);

    if (ofs)
    {
      std::copy(T_c.begin(), T_c.end(), 
          std::ostream_iterator<size_t>(ofs, "\n"));
    }
    else
    {
      perror(detectedRequestsFile);
    }
  }
}

int main( int argc, char *argv[] )
{
  std::string fileName;
  size_t quietPeriod;
  double dt, requestsDetectionAlpha;

  // Parse command line.
  try
  {
    po::options_description optDesc("Allowed options");
    optDesc.add_options()
        ("help", "display help message.")
        ("load-file-name", po::value<std::string>(&fileName), 
            "resources load statistics.")
        ("dt", po::value<double>(&dt),
            "time interval between adjacent measurements.")
        ("quiet-period", po::value<size_t>(&quietPeriod),
            "number of measurements after arrived request without requests.")
        ("requests-detection-alpha", 
            po::value<double>(&requestsDetectionAlpha),
            "request detection hypothesis confident level.")
        ;
    po::positional_options_description posOptDesc;
    posOptDesc
        .add("load-file-name", 1)
        .add("dt", 1)
        .add("quiet-period", 1)
        .add("requests-detection-alpha", 1)
        ;

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
        options(optDesc).positional(posOptDesc).run(), vm);
    po::notify(vm);

    bool const haveRequiredOptions = 
        vm.count("load-file-name") && 
        vm.count("dt") &&
        vm.count("quiet-period") &&
        vm.count("requests-detection-alpha");

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

  // Load input file.
  measurements_t measurements;
  {
    std::ifstream ifs(fileName.c_str());

    if (ifs)
    {
      std::string str;
      while (std::getline(ifs, str))
      {
        std::istringstream istr(str);
        double time, value;
        char delim;
        istr >> time >> delim >> value;
        //std::cout << time << "-" << delim << "-" << value << "\n"; // DEBUG

        measurements.push_back(value);
      }
    }
    else
    {
      perror(fileName.c_str());
      return 1;
    }
  }

  // Run estimation.
  estimate(measurements, dt, quietPeriod, requestsDetectionAlpha);

  return 0;
}

// vim: set ts=2 sw=2 et:
