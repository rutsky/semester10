#!/bin/bash

#set -x

prg=../build/src/release/main

args="../table/table.dat ../table/table2.dat"

for i in 1500 2500 4000 5000 6000 7000; do
  for gamma in 0.900 0.950 0.990 0.999; do
    for alpha in 0.100; do
      for zeta in 0.900; do
        for test_name in war_and_peace random crime_and_punishment; do
          suffix=_$test_name$gamma"_"$alpha"_"$zeta"_"$i

          echo
          echo "****************"
          echo "* Processing "$test_name with $gamma $alpha $zeta $i...
          echo "****************"
          echo cat src_$test_name"_"$i.txt \| $prg $args $gamma $alpha $zeta
          (cat src_$test_name"_"$i.txt | \
              $prg $args $gamma $alpha $zeta | \
                  stdbuf -o0 tee result$suffix.txt) || exit 1
          mv frequency1.dat result$suffix"_frequency1.dat"
          mv frequency2.dat result$suffix"_frequency2.dat"
          test -d bijections.dat && mv bijections.dat result$suffix"_bijections.dat"
        done
      done
    done
  done
done

# vim: set ts=2 sw=2 et:
