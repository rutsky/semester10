#!/bin/bash

for N in 5000; do
  for dt in 1; do
    for m in 0; do
      for sigma in 0.3; do
        for lambda in 500; do
          for m_signal in 50; do
            for sigma_signal in 1; do
              for lambda_signal in 0.1; do
                ./generate.py $N $dt $m $sigma $lambda $m_signal \
                    $sigma_signal $lambda_signal
              done
            done
          done
        done
      done
    done
  done
done

# vim: set ts=2 sw=2 et:
