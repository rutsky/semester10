#!/bin/bash

asy -f pdf requests.asy
asy -f pdf requests_num.asy
asy -f pdf noise.asy
asy -f pdf total_load.asy
asy -f pdf total_load_der.asy
asy -f pdf true_histogram.asy

asy first_load_der_surface.asy
# Interactive 3D:
# asy -V first_load_der_surface.asy