#!/bin/bash

for N in 1000; do
  for dt in 1; do
    for m in 0; do
      for sigma in 1; do
        for lambda in 0.02; do
          for m_signal in 10; do
            for sigma_signal2 in 1; do
              ./generate.py $N $dt $m $sigma $lambda $m_signal $sigma_signal2
            done
          done
        done
      done
    done
  done
done

# vim: set ts=2 sw=2 et:
