#!/bin/bash

set -x

function process()
{
  gen_prefix=$1
  load_file=$2

  for dt in 1; do
    for quiet_period in 10; do
      for request_det_alpha in 0.001; do
        for n_hist_intervals in 50; do
          for max_em_it in 100; do
            for max_em_delta in 0.01; do
              run_suffix=_quiet-period="$quiet_period"_req-det-alpha="$request_det_alpha"_hist-ints="$n_hist_intervals"_max-em-it="$max_em_it"_max-em-delta="$max_em_delta"
              echo "  --load-file-name=$load_file --quiet-period=$quiet_period --requests-detection-alpha=$request_det_alpha --histogram-intervals=$n_hist_intervals --max-em-iterations=$max_em_it --max-em-delta=$max_em_delta"
              ../build/src/release/main $load_file $dt $quiet_period \
                  $request_det_alpha $n_hist_intervals $max_em_it $max_em_delta

              ./draw_detection.sh

              for f in detected_iterative_requests.txt \
                    detected_iterative_requests.pdf \
                    detected_em_requests.txt \
                    detected_em_requests.pdf \
                    histogram.csv histogram.pdf; do
                mv $f result_${f%.*}$gen_suffix$run_suffix.${f##*.}
              done
            done
          done
        done
      done
    done
  done
}

for N in 1000; do
  for dt in 1; do
    for m in 0; do
      for sigma in 1; do
        for lambda in 0.02; do
          for m_signal in 10; do
            for sigma_signal in 1; do
              gen_suffix=_N="$N"_dt="$dt"_m="$m"_sigma="$sigma"_lambda="$lambda"_m_c="$m_signal"_sigma_c="$sigma_signal"

              ./generate.py $N $dt $m $sigma $lambda $m_signal $sigma_signal
              ./draw_graphs.sh

              #process $gen_prefix total_load.csv

              for f in requests.csv requests.pdf \
                    requests_num.csv requests_num.pdf \
                    noise.csv noise.pdf \
                    total_load.csv total_load.pdf \
                    total_load_der.csv total_load_der.pdf; do
                mv $f result_${f%.*}$gen_suffix$run_suffix.${f##*.}
              done
            done
          done
        done
      done
    done
  done
done

# vim: set ts=2 sw=2 et:
