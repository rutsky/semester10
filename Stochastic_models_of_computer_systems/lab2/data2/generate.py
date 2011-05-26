#!/usr/bin/env python
# -*- coding: utf-8 -*

import argparse
import random
import csv
import math
import operator
import itertools

import numpy

def generate_requests(N, dt, lambda_):
    return numpy.random.poisson(lambda_ * dt, N)

def generate_requests_value(dt, requests, m_signal, sigma_signal2, lambda_c):
    requests = list(requests)
    N = len(requests)
    standart_deviation = math.sqrt(sigma_signal2)
    time2req = {}
    for i, n_requests in enumerate(requests):
        t = i * dt
        for ri in xrange(n_requests):
            value = random.gauss(m_signal, standart_deviation)
            time2req.setdefault(t, []).append(value)

        load = 0
        for request_t, t_requests in time2req.iteritems():
            for request_value in t_requests:
                load += request_value * math.exp((t - request_t) * -lambda_c)
        yield load

def generate_wiener_noise(N, dt, average, scale):
    value = average
    if N >= 1:
        yield value

    for idx in xrange(N - 1):
        value += random.gauss(0, dt) * scale
        yield value

def write_stat_to_file(file_name, data):
    with open(file_name, 'wb') as f:
        writer = csv.writer(f)
        for row in data:
            writer.writerow(row)

def main():
    parser = argparse.ArgumentParser(description='Generate data for lab #2.')
    parser.add_argument('N', type=int,
            help='number of measurements')
    parser.add_argument('dt', type=float,
            help='time between measurements')
    parser.add_argument('m', type=float,
            help='noise signal expectation')
    parser.add_argument('sigma', type=float,
            help='Wiener process multiplication factor')
    parser.add_argument('lambda_', metavar='lambda', type=float,
            help='signal emerging Poisson distribution parameter '
                 '(average time between requests)')
    parser.add_argument('m_signal', type=float,
            help='signal value expectation')
    parser.add_argument('sigma_signal', type=float,
            help='signal value standart deviation')
    parser.add_argument('lambda_signal', type=float,
            help='signal value exponental decreasing coefficient')

    args = parser.parse_args()

    random.seed(0)
    numpy.random.seed(0)

    time_marks = [i * args.dt for i in xrange(args.N)]
    requests = list(generate_requests(args.N, args.dt, 
            float(args.dt) / args.lambda_))
    requests_value = list(generate_requests_value(
            args.dt, requests, args.m_signal, args.sigma_signal**2, 
            args.lambda_signal))
    wiener_noise_value = list(generate_wiener_noise(
            args.N, args.dt, args.m, args.sigma))

    result_signal = map(sum, zip(requests_value, wiener_noise_value))

    result_signal_derivative = map(lambda x: x[0] - x[1], 
            zip(result_signal[1:], result_signal))

    # Store derivative values after first request.
    (first_signal_idx, _), (second_signal_idx, _) = filter(
            lambda x: x[1], 
            zip(itertools.count(), requests))[:2]
    write_stat_to_file('first_load_der.csv', 
            zip(time_marks, 
                result_signal_derivative[
                    first_signal_idx:second_signal_idx - 1]))

    write_stat_to_file('requests.csv', zip(time_marks, requests_value))
    write_stat_to_file('requests_num.csv', zip(time_marks, requests))
    write_stat_to_file('noise.csv', zip(time_marks, wiener_noise_value))
    write_stat_to_file('total_load.csv', zip(time_marks, result_signal))
    write_stat_to_file('total_load_der.csv', 
            zip(time_marks, result_signal_derivative))

if __name__ == "__main__":
    main()

# vim: set ts=4 sw=4 et:
