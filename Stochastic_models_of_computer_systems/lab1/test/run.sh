#!/bin/bash

#set -x

prg=../build/src/main

args="../table/table.dat ../table/table2.dat 0.8 0.1 0.9"

for i in 1500; do
  for test_name in war_and_peace random crime_and_punishment; do
    cat $test_name"_"$i.txt | $prg $args > result_$test_name"_"$i.txt
    mv frequency1.dat result_$test_name"_"frequency1_$i.dat
    mv frequency2.dat result_$test_name"_"frequency2_$i.dat
  done
done

# vim: set ts=2 sw=2 et:
