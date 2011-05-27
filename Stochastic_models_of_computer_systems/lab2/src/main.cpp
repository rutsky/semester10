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

typedef std::vector<double> measurements_t;
typedef std::vector<double> derivatives_t;
typedef std::vector<size_t> request_indexes_t;
// TODO: Is using of double as key is safe? Rounding problems?
typedef std::map<double, size_t> histogram_t;

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
      double const sigma2 = accum / ((static_cast<double>(n) - 1) * dt);

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

void writeHistogram( char const *fileName, histogram_t const &histogram )
{
  std::ofstream ofs(fileName);

  if (ofs)
  {
    BOOST_FOREACH(histogram_t::value_type const &pair, histogram)
    {
      ofs << pair.first << "," << pair.second << "\n";
    }
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
      // Count measurement only at request time.
      sum += derivatives[i];
      ++j;
    }
  }

  double const d2 = sum / static_cast<double>(requests.size());

  BOOST_ASSERT(d2 - sqr(sigma) * dt);
  double const stDeviation = sqrt(d2 - sqr(sigma) * dt);

  return std::make_pair(average, stDeviation);
}

void buildHistogram( derivatives_t const &derivatives, 
                     size_t nIntervals,
                     histogram_t &histogram )
{
  BOOST_ASSERT(derivatives.size() >= 2);
  BOOST_ASSERT(nIntervals >= 2);

  histogram.clear();

  double const minDerivative = 
      *std::min_element(derivatives.begin(), derivatives.end());
  double const maxDerivative = 
      *std::max_element(derivatives.begin(), derivatives.end());

  struct ToIntervalCenter
  {
    ToIntervalCenter( size_t nIntervals, double minDer, double maxDer )
      : nIntervals_(nIntervals)
      , minDer_(minDer)
      , maxDer_(maxDer)
    {
      BOOST_ASSERT(minDer < maxDer);
    }

    double operator () ( double val ) const
    {
      BOOST_ASSERT(minDer_ <= val && val <= maxDer_);

      double const step = 
          (maxDer_ - minDer_) / static_cast<double>(nIntervals_ - 1);
      BOOST_ASSERT(step != 0);

      int const intervalIdx = 
        static_cast<int>(std::floor((val - (minDer_ - step / 2.0)) / step));
      BOOST_ASSERT(intervalIdx >= 0 && 
        static_cast<size_t>(intervalIdx) < nIntervals_);

      return minDer_ + step * intervalIdx;
    }

  private:
    size_t nIntervals_;
    double minDer_, maxDer_;
  };

  ToIntervalCenter const toCenter(nIntervals, minDerivative, maxDerivative);

  BOOST_FOREACH(double derivative, derivatives)
  {
    double const intervalCenter = toCenter(derivative);
    if (histogram.find(intervalCenter) == histogram.end())
      histogram[intervalCenter] = 0;

    ++histogram[intervalCenter];
  }
}

// Note: x_i is local maximum iff x_{i-1} <= x_i >= x_{i+1}
template< class FwdIt >
FwdIt firstHistogramLocalMax( FwdIt first, FwdIt beyond )
{
  if (first == beyond)
    return beyond;
  FwdIt pprev = first++;

  if (first == beyond)
    return beyond;
  FwdIt prev = first++;

  for (; first != beyond; pprev = prev, prev = first, ++first)
  {
    if (pprev->second <= prev->second && prev->second >= first->second)
      return prev;
  }

  return beyond;
}

void estimate( measurements_t const &measurements, double dt,
               size_t quietPeriod, double requestsDetectionAlpha,
               size_t nHistogramIntervals, size_t maxEMIterations,
               double maxDeltaEM )
{
  char const 
      *detectedIterativeRequestsFile = "detected_iterative_requests.txt",
      *detectedEMRequestsFile = "detected_em_requests.txt",
      *histogramFile = "histogram.csv";

  size_t const N = measurements.size();

  // Calculate numeric derivative.
  derivatives_t derivatives;
  calcDerivatives(measurements, derivatives);

  {
    //
    // Iterative algorithm.
    //

    std::cout << "*** Iterative method ***\n";

    // Detect times when requests arrived.
    request_indexes_t iterative_T_c;
    calcRequestsArrival(measurements, derivatives, dt, quietPeriod, 
        requestsDetectionAlpha, iterative_T_c);  
    BOOST_ASSERT(!iterative_T_c.empty());

    // Write detected requests in file.
    writeRequests(detectedIterativeRequestsFile, iterative_T_c);

    std::cout << "Detected " << iterative_T_c.size() << " requests.\n";

    // Estimate Poisson parameter.
    double const iterative_lambda = calcPoissonParameter(iterative_T_c, dt);
    std::cout << "Poisson parameter lambda: " << iterative_lambda << 
        " (average time between requests)\n";

    // Estimate noise average (m).
    double const iterative_m = calcNoiseAverage(measurements, iterative_T_c);
    std::cout << "Noise average m: " << iterative_m << 
        " (by " << iterative_T_c.front() << 
        " measurements until first requests)\n";

    // Estimate noise standard deviation (\sigma).
    double derivativeAverage, iterative_sigma;
    boost::tie(derivativeAverage, iterative_sigma) = 
      calcNoiseStDeviation(derivatives, iterative_T_c, dt);
    std::cout << "Noise standard deviation sigma: " << iterative_sigma << 
        " (derivative average: " << derivativeAverage << ")\n";

    // Estimate requests average (m_c) and standard deviation (\sigma_c).
    double iterative_m_c, iterative_sigma_c;
    boost::tie(iterative_m_c, iterative_sigma_c) = 
      calcRequestsParams(derivatives, iterative_T_c, dt, iterative_sigma);
    std::cout << "Requests average m_c: " << iterative_m_c << "\n";
    std::cout << "Requests standard deviation sigma_c: " << 
        iterative_sigma_c << "\n";
  }

  {
    //
    // EM-algorithm.
    //

    std::cout << "\n*** EM-algorithm ***\n";

    // Build histogram.
    histogram_t histogram;
    buildHistogram(derivatives, nHistogramIntervals, histogram);

    // Write histogram to file.
    writeHistogram(histogramFile, histogram);

    typedef boost::array<double, 2> parameters_array_t;

    // Calculate first and last local maximum of histogram - estimation
    // of \mu_1, \mu_2.
    parameters_array_t mu;
    {
      histogram_t::const_iterator firstLMIt = 
          firstHistogramLocalMax(histogram.begin(), histogram.end());
      BOOST_ASSERT(firstLMIt != histogram.end());
      // First local maximum is noise maximum near zero --- \mu_2.
      mu[1] = firstLMIt->first;
    }

    {
      histogram_t::const_reverse_iterator lastLMIt = 
          firstHistogramLocalMax(histogram.rbegin(), histogram.rend());
      BOOST_ASSERT(lastLMIt != histogram.rend());
      // Last local maximum is requests maximum near \m_c --- \mu_1.
      mu[0] = lastLMIt->first;
      BOOST_ASSERT(mu[1] < mu[0]);
    }

    // Other parameters initial estimation.
    parameters_array_t tau;
    tau[0] = 0.5;
    tau[1] = 0.5;
    parameters_array_t sigma;
    sigma[0] = sigma[1] = (mu[0] - mu[1]) / 3.0;

    std::cout << 
        "Start estimation: \n"
        "  mu_1 = " << mu[0] << ", mu_2 = " << mu[1] << "\n"
        "  sigma_1 = " << sigma[0] << ", sigma_2 = " << sigma[1] << "\n"
        "  tau_1 = " << tau[0] << ", tau_2 = " << tau[1] << "\n";

    // EM-algorithm.
    std::vector<parameters_array_t> T(derivatives.size());
    size_t stepIdx = 0;
    for (; stepIdx < maxEMIterations; ++stepIdx)
    {
      // E-step.

      for (size_t i = 0; i < derivatives.size(); ++i)
      {
        for (size_t j = 0; j < 2; ++j)
        {
          double const fj = 
            pdf(boost::math::normal_distribution<>(mu[j], sigma[j]), 
                derivatives[i]);
          double const f0 = 
            pdf(boost::math::normal_distribution<>(mu[0], sigma[0]),
                derivatives[i]);
          double const f1 = 
            pdf(boost::math::normal_distribution<>(mu[1], sigma[1]),
                derivatives[i]);

          double const denominator = tau[0] * f0 + tau[1] * f1;
          BOOST_ASSERT(fabs(denominator) > 1e-10);
          T[i][j] = tau[j] * fj / denominator;
          BOOST_ASSERT(T[i][j] >= 0);
          BOOST_ASSERT(T[i][j] <= 1);

          //std::cout << "(" << i << "," << j << "," << T[i][j] << ")"; // DEBUG
        }
      }
      //std::cout << "\n"; // DEBUG

      // M-step.
      parameters_array_t nextMu, nextSigma, nextTau;

      // \tau estimation.
      parameters_array_t sumT;
      {
        sumT[0] = sumT[1] = 0;
        for (size_t i = 0; i < derivatives.size(); ++i)
          for (size_t j = 0; j < 2; ++j)
            sumT[j] += T[i][j];
        BOOST_ASSERT(fabs(sumT[0]) > 1e-10);
        BOOST_ASSERT(fabs(sumT[1]) > 1e-10);

        for (size_t j = 0; j < 2; ++j)
          nextTau[j] = sumT[j] / static_cast<double>(N);
      }

      // \mu estimation.
      {
        parameters_array_t sumTx;
        sumTx[0] = sumTx[1] = 0;
        for (size_t i = 0; i < derivatives.size(); ++i)
          for (size_t j = 0; j < 2; ++j)
            sumTx[j] += T[i][j] * derivatives[i];

        for (size_t j = 0; j < 2; ++j)
          nextMu[j] = sumTx[j] / sumT[j];
      }

      // \sigma estimation.
      {
        parameters_array_t sumTxmu;
        sumTxmu[0] = sumTxmu[1] = 0;
        for (size_t i = 0; i < derivatives.size(); ++i)
          for (size_t j = 0; j < 2; ++j)
            sumTxmu[j] += T[i][j] * sqr(derivatives[i] - nextMu[j]);

        for (size_t j = 0; j < 2; ++j)
          nextSigma[j] = sumTxmu[j] / sumT[j];

        // HACK: Limit standard deviation so it will never be too close to 
        // zero.
        //for (size_t j = 0; j < 2; ++j)
        //  nextSigma[j] = std::max(nextSigma[j], 0.05);
      }
      
      // DEBUG
      std::cout << stepIdx << ") "
        "nextTau = (" << nextTau[0] << "," << nextTau[1] << "), "
        "nextMu = (" << nextMu[0] << "," << nextMu[1] << "), "
        "nextSigma = (" << nextSigma[0] << "," << nextSigma[1] << ")\n";

      bool smaller(true);
      for (size_t j = 0; j < 2; ++j)
        if (std::fabs(nextTau[j] - tau[j]) > maxDeltaEM ||
            std::fabs(nextMu[j] - mu[j]) > maxDeltaEM ||
            std::fabs(nextSigma[j] - sigma[j]) > maxDeltaEM)
        {
          smaller = false;
          break;
        }

      // Assign new value.
      for (size_t j = 0; j < 2; ++j)
      {
        tau[j] = nextTau[j];
        mu[j] = nextMu[j];
        sigma[j] = nextSigma[j];
      }

      // Break if approximate limit reached.
      if (smaller)
        break;
    }

    // Evaluate request arrival time.
    request_indexes_t em_T_c;
    for (size_t i = 0; i < T.size(); ++i)
      if (T[i][0] > T[i][1])
      {
        // Request arrived;
        em_T_c.push_back(i);
      }

    // Write detected requests in file.
    writeRequests(detectedEMRequestsFile, em_T_c);

    std::cout << "Detected " << em_T_c.size() << " requests.\n";

    // Estimate Poisson parameter.
    double const em_lambda = calcPoissonParameter(em_T_c, dt);
    std::cout << "Poisson parameter lambda: " << em_lambda << 
        " (average time between requests)\n";

    // Estimate noise average (m).
    double const em_m = calcNoiseAverage(measurements, em_T_c);
    std::cout << "Noise average m: " << em_m << 
        " (by " << em_T_c.front() << 
        " measurements until first requests)\n";

    // Estimate noise standard deviation (\sigma).
    double em_sigma = sigma[1] / sqrt(dt);
    double derivativeAverage = mu[1];
    std::cout << "Noise standard deviation sigma: " << em_sigma << 
        " (derivative average: " << derivativeAverage << ")\n";

    // Estimate requests average (m_c) and standard deviation (\sigma_c).
    double em_m_c = mu[0];
    double em_sigma_c = sqrt(sqr(sigma[0]) - sqr(em_sigma) * dt);
    std::cout << "Requests average m_c: " << em_m_c << "\n";
    std::cout << "Requests standard deviation sigma_c: " << 
        em_sigma_c << "\n";
  }
}

int main( int argc, char *argv[] )
{
  std::string fileName;
  size_t quietPeriod;
  double dt, requestsDetectionAlpha;
  size_t nHistogramIntervals, maxEMIterations;
  double maxDeltaEM;

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
        ("histogram-intervals", 
            po::value<size_t>(&nHistogramIntervals),
            "number of intervals used in histogram.")
        ("max-em-iterations", 
            po::value<size_t>(&maxEMIterations),
            "maximum number of iteration in EM-algorithm.")
        ("max-em-delta", 
            po::value<double>(&maxDeltaEM),
            "EM-algorithm loop stop parameter.")
        ;
    po::positional_options_description posOptDesc;
    posOptDesc
        .add("load-file-name", 1)
        .add("dt", 1)
        .add("quiet-period", 1)
        .add("requests-detection-alpha", 1)
        .add("histogram-intervals", 1)
        .add("max-em-iterations", 1)
        .add("max-em-delta", 1)
        ;

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
        options(optDesc).positional(posOptDesc).run(), vm);
    po::notify(vm);

    bool const haveRequiredOptions = 
        vm.count("load-file-name") && 
        vm.count("dt") &&
        vm.count("quiet-period") &&
        vm.count("requests-detection-alpha") &&
        vm.count("histogram-intervals") &&
        vm.count("max-em-iterations") &&
        vm.count("max-em-delta");

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
  estimate(measurements, dt, quietPeriod, requestsDetectionAlpha,
      nHistogramIntervals, maxEMIterations, maxDeltaEM);

  return 0;
}

// vim: set ts=2 sw=2 et:
