#!/bin/bash

set -x

../build/src/release/main total_load.csv 1 10 0.001 50 100 0.01
