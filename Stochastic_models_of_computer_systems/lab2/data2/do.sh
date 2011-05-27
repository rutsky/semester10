#!/bin/bash

#set -x

function process()
{
  gen_prefix=$1
  load_file="total_load.csv"

  for dt in 1; do
    for pick_part in 0.003; do
      for join_dist in 4; do
        run_suffix=_p-p="$pick_part"_j-d="$join_dist"
        echo "Run: --load-file-name=$load_file --dt=1 --var-series-pick-part=$pick_part --join-dist=$join_dist"
        ../build/src2/release/main $load_file $dt $pick_part $join_dist \
            | tee result$gen_suffix$run_suffix"_output.txt"

        ./draw_detection.sh

        for f in detected_requests.txt \
              detected_requests.pdf \
              ; do
          mv $f result$gen_suffix$run_suffix"_"$f
        done
      done
    done
  done
}

for N in 5000; do
  for dt in 1; do
    for m in 0; do
      for sigma in 0.3; do
        for lambda in 500; do
          for m_signal in 50; do
            for sigma_signal in 1; do
              for lambda_signal in 0.1; do
                gen_suffix=_N="$N"_dt="$dt"_m="$m"_s="$sigma"_l="$lambda"_m_c="$m_signal"_s_c="$sigma_signal"_l_c="$lambda_signal"

                echo "**********"
                echo "Generating: "$gen_suffix
                echo

                ./generate.py $N $dt $m $sigma $lambda $m_signal \
                    $sigma_signal $lambda_signal

                #./draw_graphs.sh

                process "$gen_prefix" total_load.csv

                for f in requests.csv requests.pdf \
                      requests_num.csv requests_num.pdf \
                      noise.csv noise.pdf \
                      total_load.csv total_load.pdf \
                      total_load_der.csv total_load_der.pdf \
                      true_histogram.pdf; do
                  mv $f result"$gen_suffix"_$f
                done

                echo
              done
            done
          done
        done
      done
    done
  done
done

# vim: set ts=2 sw=2 et:
