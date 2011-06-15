#!/bin/bash

#set -x

export LC_NUMERIC=C

dt=1
N=5000
base_m=0
base_sigma=0.3
base_lambda=500 # actually 1/lambda
base_m_signal=50
base_sigma_signal=1
base_lambda_signal=0.1

params_file=gen_params.csv
load_file=total_load.csv
estimated_params_file=estimated_params.txt
prg=../build/src2/release/main

function generate()
{
  n_requests=`./generate.py $1 $2 $3 $4 $5 $6 $7 $8 | grep -o -E 'Number of requests: ([0-9]+)' | sed -r 's/Number of requests: ([0-9]+)/\1/'`
  echo -n $1,$2,$n_requests,$3,$4,$5,$6,$7,$8 >> $params_file
}

echo "**********"
echo -n > $params_file
for sigma in `seq 0.3 0.1 0.8`; do
  generate $N $dt $base_m $sigma $base_lambda $base_m_signal \
      $base_sigma_signal $base_lambda_signal

  $prg $load_file
  cat $estimated_params_file >> $params_file

  echo
  echo
done



# vim: set ts=2 sw=2 et:
