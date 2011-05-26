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
#include <map>

#include <boost/program_options.hpp>
#include <boost/foreach.hpp>
#include <boost/math/distributions/normal.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/array.hpp>

namespace po = boost::program_options;

template< class T >
T sqr( T const &v )
{
  return v * v;
}

struct compare_second_t
{
  template< class T >
  bool operator()( T const &left, T const &right ) const
  {
    return left.second < right.second;
  }
};

typedef std::vector<double> measurements_t;
typedef std::vector<double> derivatives_t;
typedef std::vector<size_t> request_indexes_t;
// TODO: Is using of double as key is safe? Rounding problems?
typedef std::multimap<double, size_t> variational_series_t;

void calcDerivatives( measurements_t const &measurements, 
                      derivatives_t &derivatives )
{
  derivatives.clear();
  derivatives.reserve(measurements.size());
  derivatives.push_back(0);
  for (size_t i = 1; i < measurements.size(); ++i)
    derivatives.push_back(measurements[i] - measurements[i - 1]);
}

void calcVariationalSeries( measurements_t const &measurements,
                            variational_series_t &varseries )
{
  varseries.clear();
  for (size_t i = 0; i < measurements.size(); ++i)
    varseries.insert(std::make_pair(measurements[i], i));
}

void calcRequestsArrival( variational_series_t const &varseries,
                          double varSeriesPickPart,
                          size_t joinDist,
                          request_indexes_t &requests )
{
  BOOST_ASSERT(0 < varSeriesPickPart && varSeriesPickPart < 1);
  BOOST_ASSERT(joinDist >= 2);

  requests.clear();

  // Get right part of variational series.
  variational_series_t vs;
  {
    variational_series_t::const_reverse_iterator it = varseries.rbegin();
    std::advance(it, 
      static_cast<int>(
        static_cast<double>(varseries.size()) * varSeriesPickPart));
    std::copy(
        varseries.rbegin(),
        it, 
        std::inserter(vs, vs.end()));
  }

  /*
  std::cout << vs.size() << "\n";
  BOOST_FOREACH(variational_series_t::value_type const &p, varseries)
    std::cout << "(" << p.first << "," << p.second << ")";
  std::cout << "\n";*/

  // Iteratively select requests.
  while (!vs.empty())
  {
    // Locate left-most item --- request.
    variational_series_t::iterator it = 
        std::min_element(vs.begin(), vs.end(), compare_second_t());
    size_t idx = it->second;
    requests.push_back(idx);
    vs.erase(it);

    // Remove close to request values.
    while (!vs.empty())
    {
      bool removed = false;
      for (it = vs.begin(); it != vs.end();)
      {
        if (it->second < idx + joinDist)
        {
          if (it->second > idx)
            idx = it->second;

          variational_series_t::iterator eraseIt = it++;
          vs.erase(eraseIt);

          removed = true;
        }
        else
        {
          ++it;
        }
      }

      if (!removed)
        break;
    }
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

double calcNoiseAverage( measurements_t const &measurements,
                         request_indexes_t const &requests )
{
  BOOST_ASSERT(requests.size() < measurements.size());
  
  double sum(0);
  size_t i = 0;
  for (; i < measurements.size(); ++i)
  {
    if (!requests.empty() && requests.front() == i)
    {
      // Stop at request.
      break;
    }

    sum += measurements[i];
  }
  if (i > 0)
  {
    return sum / static_cast<double>(i);
  }
  else
  {
    // TODO
    return 0;
  }
}

double calcPoissonParameter( request_indexes_t const &requests, double dt )
{
  BOOST_ASSERT(requests.size() >= 2);

  double sum(0);
  for (size_t i = 1; i < requests.size(); ++i)
    sum += dt * static_cast<double>(requests[i] - requests[i - 1]);
  return sum / static_cast<double>(requests.size() - 1);
}

std::pair<double, double> 
    calcNoiseStDeviation( derivatives_t const &derivatives,
                          request_indexes_t const &requests,
                          double dt )
{
  BOOST_ASSERT(requests.size() < derivatives.size());

  double sum(0);
  for (size_t i = 0, j = 0; i < derivatives.size(); ++i)
  {
    if (j < requests.size() && requests[j] == i)
    {
      // Skip measurement at request time.
      //std::cout << derivatives[i] << "\n"; // DEBUG
      ++j;
      continue;
    }

    sum += derivatives[i];
  }

  double const average = 
      sum / static_cast<double>(derivatives.size() - requests.size());

  sum = 0;
  for (size_t i = 0, j = 0; i < derivatives.size(); ++i)
  {
    if (j < requests.size() && requests[j] == i)
    {
      // Skip measurement at request time.
      ++j;
      continue;
    }

    sum += sqr(derivatives[i] - average);
  }

  double const d2 = sum / 
      static_cast<double>(derivatives.size() - requests.size());

  double const stDeviation = sqrt(d2 / dt);

  return std::make_pair(average, stDeviation);
}

std::pair<double, double> 
    calcRequestsParams( derivatives_t const &derivatives,
                        request_indexes_t const &requests,
                        double dt, double sigma )
{
  BOOST_ASSERT(requests.size() < derivatives.size());
  BOOST_ASSERT(requests.size() > 0);

  double sum(0);
  for (size_t i = 0, j = 0; i < derivatives.size(); ++i)
  {
    if (j < requests.size() && requests[j] == i)
    {
      // Count measurement only at request time.
      sum += derivatives[i];
      ++j;
    }
  }

  double const average = sum / static_cast<double>(requests.size());

  sum = 0;
  for (size_t i = 0, j = 0; i < derivatives.size(); ++i)
  {
    if (j < requests.size() && requests[j] == i)
    {
      // Count measurement only at request tim`e.
      sum += derivatives[i];
      ++j;
    }
  }

  double const d2 = sum / static_cast<double>(requests.size());

  BOOST_ASSERT(d2 - sqr(sigma) * dt);
  double const stDeviation = sqrt(d2 - sqr(sigma) * dt);

  return std::make_pair(average, stDeviation);
}

void estimate( measurements_t const &measurements, double dt,
               double varSeriesPickPart, size_t joinDist )
{
  char const 
      *detectedRequestsFile = "detected_requests.txt";

  size_t const N = measurements.size();

  // Calculate numeric derivative.
  derivatives_t derivatives;
  calcDerivatives(measurements, derivatives);

  // Build variational series.
  variational_series_t varseries;
  calcVariationalSeries(derivatives, varseries);

  // Estimate requests arrival time.
  request_indexes_t T_c;
  calcRequestsArrival(varseries, varSeriesPickPart, joinDist, T_c);

  // Write detected requests in file.
  writeRequests(detectedRequestsFile, T_c);

  std::cout << "Detected " << T_c.size() << " requests.\n";
}

int main( int argc, char *argv[] )
{
  std::string fileName;
  double dt, varSeriesPickPart;
  size_t joinDist;

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
        ("var-series-pick-part", po::value<double>(&varSeriesPickPart),
            "right end normalized size of variational series which will be"
            "analyzed for requests (from 0 to 1).")
        ("join-dist", 
            po::value<size_t>(&joinDist),
            "requests detection join distance.")
        ;
    po::positional_options_description posOptDesc;
    posOptDesc
        .add("load-file-name", 1)
        .add("dt", 1)
        .add("var-series-pick-part", 1)
        .add("join-dist", 1)
        ;

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
        options(optDesc).positional(posOptDesc).run(), vm);
    po::notify(vm);

    bool const haveRequiredOptions = 
        vm.count("load-file-name") && 
        vm.count("dt") &&
        vm.count("var-series-pick-part") &&
        vm.count("join-dist") &&
        true;

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
  estimate(measurements, dt, varSeriesPickPart,
      joinDist);

  return 0;
}

// vim: set ts=2 sw=2 et:
