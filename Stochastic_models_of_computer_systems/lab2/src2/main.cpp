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
#include <vector>
#include <map>
#include <cmath>

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

template< class T >
int sign( T const &v )
{
  if (v < 0)
    return -1;
  else if (v > 0)
    return +1;
  else
    return 0;
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
                          double dt,
                          size_t quietPeriod,
                          double requestsDetectionAlpha,
                          request_indexes_t &requests )
{
  BOOST_ASSERT(quietPeriod >= 2);
  BOOST_ASSERT(dt >= 1e-8);
  BOOST_ASSERT(0 <= requestsDetectionAlpha && requestsDetectionAlpha <= 1);

  requests.clear();

  // DEBUG
  /*BOOST_FOREACH(variational_series_t::value_type const &p, varseries)
    std::cout << "(" << p.first << "," << p.second << ")";
  std::cout << "\n";*/
  // END OF DEBUG

  variational_series_t::const_reverse_iterator it = varseries.rbegin();

  // Estimate \sigma^2 on quiet period.
  double muAccum(0), sigmaAccum(0);
  size_t n = 1;
  for (; n < quietPeriod && it != varseries.rend(); ++n, ++it)
  {
    muAccum += it->first;
    sigmaAccum += sqr(it->first);
    requests.push_back(it->second);
  }

  for (; it != varseries.rend(); 
      muAccum += it->first, sigmaAccum += sqr(it->first), ++it, ++n)
  {
    double const mu = muAccum / (static_cast<double>(n) * dt);
    double const sigma2 = sigmaAccum / ((static_cast<double>(n) - 1) * dt);

    // Construct normal distribution for currently estimated \sigma^2
    boost::math::normal_distribution<> 
        normalDistr(mu, std::sqrt(sigma2 * dt));
    double const loQuantile = 
        quantile(normalDistr, 
            requestsDetectionAlpha / 2.0);
    double const hiQuantile = 
        quantile(complement(
            normalDistr, 
            requestsDetectionAlpha / 2.0));

    // DEBUG
    std::cout << n << " ";
    std::cout << "  sigma2: " << sigma2 << 
      ", loQuantile: " << loQuantile <<
      ", hiQuantile: " << hiQuantile << 
      ", det: " << it->first << "\n";
    // END OF DEBUG
    
    // Check if next observing value lies in rare quantiles.
    if (it->first >= loQuantile && it->first <= hiQuantile)
    {
      // Request.
      requests.push_back(it->second);
    }
    else
      break;
  }

  std::sort(requests.begin(), requests.end());
}

template< class DerFwdIt >
double sum( DerFwdIt first, DerFwdIt beyond, double dt, double N, double lc )
{
  double sum = 0;

  for (size_t i = 0; first != beyond; ++first, ++i)
  {
    double const ti = dt * static_cast<double>(i);
    double const yi = *first;
    sum += sqr(yi - N * (exp(-lc * (ti + dt)) - exp(-lc * ti)));
  }

  return sum;
}

template< class DerFwdIt >
double dN( DerFwdIt first, DerFwdIt beyond, double dt, double N, double lc )
{
  double dsum_dN = 0;

  for (size_t i = 0; first != beyond; ++first, ++i)
  {
    double const ti = dt * static_cast<double>(i);
    double const yi = *first;
    dsum_dN += 
        -2 * (yi - N * (exp(-lc * (ti + dt)) - exp(-lc * ti))) * 
        (exp(-lc * (ti + dt)) - exp(-lc * ti));
  }

  return dsum_dN;
}

template< class DerFwdIt >
double dlc( DerFwdIt first, DerFwdIt beyond, double dt, double N, double lc )
{
  double dsum_dlc = 0;

  for (size_t i = 0; first != beyond; ++first, ++i)
  {
    double const ti = dt * static_cast<double>(i);
    double const yi = *first;
    dsum_dlc += 
        -2 * (yi - N * (exp(-lc * (ti + dt)) - exp(-lc * ti))) * N *
        (-(ti + dt) * exp(-lc * (ti + dt)) + ti * exp(-lc * ti));
  }

  return dsum_dlc;
}

template< class DerFwdIt >
void calcExpParameters( DerFwdIt first, DerFwdIt beyond, double dt, 
                        double startStepN, double startStepLc )
{
  // TODO: Start values should be estimater somehow.
  double N = 60;
  double lc = 0.2;

  size_t const binSearchSteps = 12;
  double const NApproxDist = 1e-3;
  double const lcApproxDist = 1e-3;

  /*
  {
    // DEBUG
    std::cout << "yi = ";
    size_t i = 0;
    for (DerFwdIt it = first; it != beyond; ++it, ++i)
      std::cout << i << ") " << *it << "\n";
    std::cout << "\n";
  }
  */

  for (size_t idx = 0; ; ++idx)
  {
    // Search minimum on OX axis (N).
    double NStep;
    {
      double const NDer = dN(first, beyond, dt, N, lc);

      std::cout << "Searching minimum by N: N=" << N << ", lc=" << lc <<
          ", dN=" << NDer << "\n";

      double const dir = -sign(NDer);
      double l(0), r(startStepN);
      for (size_t i = 0; i < binSearchSteps; ++i)
      {
        double const lN = N + dir * l;
        double const ls = sum(first, beyond, dt, lN, lc);
        std::cout << " -- lN=" << lN << " ls=" << ls << "\n";
      
        double const rN = N + dir * r;
        double const rs = sum(first, beyond, dt, rN, lc);
        std::cout << " -- rN=" << rN << " rs=" << rs << "\n";

        double const ll = l + (r - l) * 1 / 3.0;
        double const llN = N + dir * ll;
        double const lls = sum(first, beyond, dt, llN, lc);
        std::cout << " -- llN=" << llN << " lls=" << lls << "\n";

        double const rr = l + (r - l) * 2 / 3.0;
        double const rrN = N + dir * rr;
        double const rrs = sum(first, beyond, dt, rrN, lc);
        std::cout << " -- rrN=" << rrN << " rrs=" << rrs << "\n";
      
        // Because function is convex.
        BOOST_ASSERT(lls <= ls || lls <= rs);
        BOOST_ASSERT(rrs <= ls || rrs <= rs);

        if (ls >= rs)
        {
          BOOST_ASSERT(lls >= rrs);
          l = ll;
        }
        else
        {
          BOOST_ASSERT(lls <= rrs);
          r = rr;
        }
      }

      NStep = dir * l;
    }

    N += NStep;

    if (fabs(NStep) < NApproxDist /*&& RLen < lcApproxDist */)
      break;
  }

  std::cout << "lambda_c=" << lc << ", N=" << N << "\n";
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

void estimate( measurements_t const &measurements, double dt )
{
  char const 
      *detectedRequestsFile = "detected_requests.txt";

  // TODO:
  size_t const quietPeriod = 5;
  double const requestsDetectionAlpha = 0.8;
  double const startStepN = 10;
  double const startStepLc = 0.05;

  //size_t const N = measurements.size();

  // Calculate numeric derivative.
  derivatives_t derivatives;
  calcDerivatives(measurements, derivatives);

  // Build variational series.
  variational_series_t varseries;
  calcVariationalSeries(derivatives, varseries);

  // Estimate requests arrival time.
  request_indexes_t T_c;
  calcRequestsArrival(varseries, dt, quietPeriod, requestsDetectionAlpha, T_c);

  // Write detected requests in file.
  writeRequests(detectedRequestsFile, T_c);

  std::cout << "Detected " << T_c.size() << " requests.\n";

  // Estimate parameters for each period between requests.
  for (size_t i = 1; i < T_c.size(); ++i)
  {
    calcExpParameters(
        derivatives.begin() + T_c[i - 1] + 1, 
        derivatives.begin() + T_c[i], dt, startStepN, startStepLc);
    std::cout << "\n";

    break; // DEBUG
  }
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
  estimate(measurements, dt);

  return 0;
}

// vim: set ts=2 sw=2 et:
