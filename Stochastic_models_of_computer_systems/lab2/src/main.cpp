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
typedef std::vector<double> derivatives_t;
typedef std::vector<size_t> request_indexes_t;

void calcDerivatives( measurements_t const &measurements, 
                      derivatives_t &derivatives )
{
  derivatives.clear();
  derivatives.reserve(measurements.size());
  derivatives.push_back(0);
  for (size_t i = 1; i < measurements.size(); ++i)
    derivatives.push_back(measurements[i] - measurements[i - 1]);
}

void calcRequestsArrival( measurements_t const &measurements,
                          derivatives_t const &derivatives,
                          double dt,
                          size_t quietPeriod,
                          double requestsDetectionAlpha,
                          request_indexes_t &requests )
{
  BOOST_ASSERT(quietPeriod >= 2);
  BOOST_ASSERT(dt >= 1e-8);
  BOOST_ASSERT(0 <= requestsDetectionAlpha && requestsDetectionAlpha <= 1);

  requests.clear();

  size_t lastRequestIdx = 0;
  while (lastRequestIdx + 1 + quietPeriod + 1 < measurements.size())
  {
    // Estimate \sigma^2 on quiet period (period after request in 
    // which no request arrived).
    double accum(0);
    for (size_t i = 0; i < quietPeriod; ++i)
      accum += sqr(derivatives[lastRequestIdx + 1 + i]);

    size_t n = quietPeriod;

    size_t idx = lastRequestIdx + 1 + quietPeriod;
    BOOST_ASSERT(idx < measurements.size());

    bool foundRequest(false);
    for (; idx < measurements.size(); 
        accum += sqr(derivatives[idx]), ++idx, ++n)
    {
      double const sigma2 = accum / ((n - 1) * dt);

      // Construct normal distribution for currently estimated \sigma^2
      boost::math::normal_distribution<> 
          normalDistr(0, std::sqrt(sigma2 * dt));
      double const loQuantile = 
          quantile(normalDistr, 
              requestsDetectionAlpha / 2.0);
      double const hiQuantile = 
          quantile(complement(
              normalDistr, 
              requestsDetectionAlpha / 2.0));

      // DEBUG
      /*
      std::cout << idx << " ";
      std::cout << "  sigma2: " << sigma2 << 
        ", loQuantile: " << loQuantile <<
        ", hiQuantile: " << hiQuantile << 
        ", det: " << der[idx] << "\n";
      */
      // END OF DEBUG
      
      // Check if next observing value lies in rare quantiles.
      if (derivatives[idx] < loQuantile || derivatives[idx] > hiQuantile)
      {
        // Rare event happened - request.
        //std::cout << "***\n"; // DEBUG
        foundRequest = true;
        requests.push_back(idx);
        lastRequestIdx = idx;
        break;
      }
    }

    if (!foundRequest)
      break;
  }
}

void writeRequests( char const *fileName, request_indexes_t const &requests )
{
  std::ofstream ofs(fileName);

  if (ofs)
  {
    std::copy(requests.begin(), requests.end(), 
        std::ostream_iterator<size_t>(ofs, "\n"));
  }
  else
  {
    perror(fileName);
  }
}

void estimate( measurements_t const &measurements, double dt,
               size_t quietPeriod, double requestsDetectionAlpha )
{

  char const *detectedRequestsFile = "detected_requests.txt";

  // Calculate numeric derivative.
  derivatives_t derivatives;
  calcDerivatives(measurements, derivatives);

  // Detect times when requests arrived.
  std::vector<size_t> T_c;
  calcRequestsArrival(measurements, derivatives, dt, quietPeriod, 
      requestsDetectionAlpha, T_c);  

  // Write detected requests in file.
  writeRequests(detectedRequestsFile, T_c);

  std::cout << "Detected " << T_c.size() << " requests.\n";
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
