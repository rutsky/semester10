#!/bin/bash

#set -x

stat="stat.csv"
rm $stat

function process()
{
  gen_prefix=$1
  load_file="total_load.csv"
  sigma=$3

  for dt in 1; do
    for quiet_period in 10; do
      for request_det_alpha in 0.001; do
        for n_hist_intervals in 50; do
          for max_em_it in 100; do
            for max_em_delta in 0.0001; do
              run_suffix=_q-p="$quiet_period"_req-det-a="$request_det_alpha"_hist-ints="$n_hist_intervals"_max-em-it="$max_em_it"_max-em-d="$max_em_delta"
              echo "Run: --load-file-name=$load_file --quiet-period=$quiet_period --requests-detection-alpha=$request_det_alpha --histogram-intervals=$n_hist_intervals --max-em-iterations=$max_em_it --max-em-delta=$max_em_delta"

              output=result$gen_suffix$run_suffix"_output.txt"

              ../build/src/release/main $load_file $dt $quiet_period \
                  $request_det_alpha $n_hist_intervals $max_em_it \
                  $max_em_delta | tee $output

              #./draw_detection.sh

              for f in detected_iterative_requests.txt \
                    detected_iterative_requests.pdf \
                    detected_em_requests.txt \
                    detected_em_requests.pdf \
                    histogram.csv histogram.pdf; do
                mv $f result$gen_suffix$run_suffix"_"$f
              done

              echo -n $sigma"," >> $stat
              grep -e 'Detected [0-9]* requests' -o $output | tr -d 'A-Za-z ' | tr '\n' ',' | sed -r 's/,$//' >> $stat
              echo >> $stat
            done
          done
        done
      done
    done
  done
}

for N in 10000; do
  for dt in 1; do
    for m in 0; do
      for sigma in `seq 1.3 0.01 1.45`; do
      #for sigma in `seq 1`; do
        sigma=`echo $sigma | tr ',' '.'`
        for lambda in 50; do
          for m_signal in 10; do
            #for sigma_signal in `seq 1.3 0.01 1.45`; do
            for sigma_signal in 1; do
              sigma_signal=`echo $sigma_signal | tr ',' '.'`
              gen_suffix=_N="$N"_dt="$dt"_m="$m"_s="$sigma"_l="$lambda"_m_c="$m_signal"_s_c="$sigma_signal"

              echo "**********"
              echo "Generating: "$gen_suffix
              echo

              ./generate.py $N $dt $m $sigma $lambda $m_signal $sigma_signal
              #./draw_graphs.sh

              process "$gen_prefix" total_load.csv $sigma_signal

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

# vim: set ts=2 sw=2 et:
