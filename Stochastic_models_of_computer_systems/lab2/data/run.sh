#!/bin/bash

#set -x

for load_file in total_load.csv; do
  for dt in 1; do
    for quiet_period in 10; do
      for request_det_alpha in 0.001; do
        for n_hist_intervals in 50; do
          for max_em_it in 100; do
            for max_em_delta in 0.01; do
              echo "  --load-file-name=$load_file --dt=$dt --quiet-period=$quiet_period --requests-detection-alpha=$request_det_alpha --histogram-intervals=$n_hist_intervals --max-em-iterations=$max_em_it --max-em-delta=$max_em_delta"
              ../build/src/release/main $load_file $dt $quiet_period \
                  $request_det_alpha $n_hist_intervals $max_em_it $max_em_delta
            done
          done
        done
      done
    done
  done
done

# vim: set ts=2 sw=2 et:
