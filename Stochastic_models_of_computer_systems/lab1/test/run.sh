#!/bin/bash

prg=../build/src/main

args="../table/table.dat ../table/table2.dat 0.9 0.1 0.9"

i=1500

cat war_and_peace_$i.txt | $prg $args > result_war_and_peace_$i.txt
cat random_$i.txt | $prg $args > result_random_$i.txt
cat crime_and_punishment_$i.txt | $prg $args > result_crime_and_punishment_$i.txt

# vim: set ts=2 sw=2 et
