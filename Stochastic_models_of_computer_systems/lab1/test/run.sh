#!/bin/bash

#set -x

prg=../build/src/main

args="../table/table.dat ../table/table2.dat"

for i in 1500 2500 4000 5000 6000 7000; do
  for gamma in 0.900 0.950 0.990 0.999; do
    for alpha in 0.100; do
      for zeta in 0.900; do
        for test_name in war_and_peace random crime_and_punishment; do
          suffix=_$gamma"_"$alpha"_"$zeta"_"$i
          cat src_$test_name"_"$i.txt | \
              $prg $args $gamma $alpha $zeta > \
              result_$test_name$suffix.txt
          mv frequency1.dat result_$test_name$suffix"_frequency1".dat
          mv frequency2.dat result_$test_name$suffix"_frequency2".dat
        done
      done
    done
  done
done

# vim: set ts=2 sw=2 et:
