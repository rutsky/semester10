#!/usr/bin/env python
# -*- coding: utf-8 -*

import argparse
import random
import csv
import math
import operator

import numpy

def generate_requests(N, dt, lambda_):
    return numpy.random.poisson(lambda_ * dt, N)

def generate_requests_value(requests, m_signal, sigma_signal2):
    standart_deviation = math.sqrt(sigma_signal2)
    value = 0
    for request in requests:
        for req_idx in xrange(request):
            value += random.gauss(m_signal, standart_deviation)
        yield value

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
                 '(average number of requests per one second)')
    parser.add_argument('m_signal', type=float,
            help='signal value expectation')
    parser.add_argument('sigma_signal2', type=float,
            help='signal value dispersion')

    args = parser.parse_args()

    random.seed(0)
    numpy.random.seed(0)

    time_marks = [i * args.dt for i in xrange(args.N)]
    requests = list(generate_requests(args.N, args.dt, args.lambda_))
    requests_value = list(generate_requests_value(
            requests, args.m_signal, args.sigma_signal2))
    wiener_noise_value = list(generate_wiener_noise(
            args.N, args.dt, args.m, args.sigma))

    result_signal = map(sum, zip(requests_value, wiener_noise_value))

    result_signal_derivative = map(lambda x: x[0] - x[1], 
            zip(result_signal[1:], result_signal))

    write_stat_to_file('requests.csv', zip(time_marks, requests_value))
    write_stat_to_file('requests_num.csv', zip(time_marks, requests))
    write_stat_to_file('noise.csv', zip(time_marks, wiener_noise_value))
    write_stat_to_file('total_load.csv', zip(time_marks, result_signal))
    write_stat_to_file('total_load_der.csv', 
            zip(time_marks, result_signal_derivative))

if __name__ == "__main__":
    main()

# vim: set ts=4 sw=4 et:
