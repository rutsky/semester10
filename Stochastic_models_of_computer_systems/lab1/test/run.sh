#!/bin/bash

#set -x

prg=../build/src/main

args="../table/table.dat ../table/table2.dat"

for i in 1500 2500 4000 5000; do
  for gamma in 0.9 0.95 0.99; do
    for alpha in 0.1; do
      for zeta in 0.9; do
        for test_name in war_and_peace random crime_and_punishment; do
          suffix=_$test_name"_"$gamma"_"$alpha"_"$zeta"_"$i
          cat $test_name"_"$i.txt | \
              $prg $args $gamma $alpha $zeta > \
              result$suffix.txt
          mv frequency1.dat result_frequency1$suffix.dat
          mv frequency2.dat result_frequency2$suffix.dat
        done
      done
    done
  done
done

# vim: set ts=2 sw=2 et:
