#!/bin/bash

#set -x

export LC_NUMERIC=C

dt=1
N=10000
base_m=10
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
  echo -n $1,$2,$n_requests,$3,$4,$5,$6,$7,$8, >> $params_file
}

if `false`; then
  echo "********** sigma"
  echo -n > $params_file
  for v in `seq 0.3 0.3 7.0`; do
    echo "  = sigma=$v ="

    generate $N $dt $base_m $v $base_lambda $base_m_signal \
        $base_sigma_signal $base_lambda_signal

    $prg $load_file
    cat $estimated_params_file >> $params_file

    echo
    echo
  done
  asy -f pdf gen_params_stats_sigma.asy
fi

if `false`; then
  echo "**********"
  echo -n > $params_file
  for v in `seq 200 50 1000`; do
    echo "  = lambda=$v ="

    generate $N $dt $base_m $base_sigma $v $base_m_signal \
        $base_sigma_signal $base_lambda_signal

    $prg $load_file
    cat $estimated_params_file >> $params_file

    echo
    echo
  done
  asy -f pdf gen_params_stats_lambda.asy
fi

if `false`; then
  echo "**********"
  echo -n > $params_file
  for v in `seq 20 10 200`; do
    echo "  = m_signal=$v ="

    generate $N $dt $base_m $base_sigma $base_lambda $v \
        $base_sigma_signal $base_lambda_signal

    $prg $load_file
    cat $estimated_params_file >> $params_file

    echo
    echo
  done
  asy -f pdf gen_params_stats_m_signal.asy
fi

if `false`; then
  echo "**********"
  echo -n > $params_file
  for v in `seq 1 0.5 5.0`; do
    echo "  = sigma_signal=$v ="

    generate $N $dt $base_m $base_sigma $base_lambda $base_m_signal \
        $v $base_lambda_signal

    $prg $load_file
    cat $estimated_params_file >> $params_file

    echo
    echo
  done
  asy -f pdf gen_params_stats_sigma_signal.asy
fi

if `true`; then
  echo "**********"
  echo -n > $params_file
  for v in `seq 0.05 0.025 0.2`; do
    echo "  = lambda_signal=$v ="

    generate $N $dt $base_m $base_sigma $base_lambda $base_m_signal \
        $base_sigma_signal $v

    $prg $load_file
    cat $estimated_params_file >> $params_file

    echo
    echo
  done
  asy -f pdf gen_params_stats_lambda_signal.asy
fi

# vim: set ts=2 sw=2 et:
