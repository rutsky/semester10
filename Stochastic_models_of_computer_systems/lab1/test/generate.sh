#!/bin/bash

offset=10000
tests_dir="../data/"

for i in 500 1000 1500 2500 4000 5000 6000 7000 8000 9000 10000 12000 15000 20000; do
  python gen_random.py $i > src_random_$i.txt

  for test in war_and_peace crime_and_punishment; do
    cat $tests_dir$test.txt | sed 's/[^A-Za-z ]/ /g' | tr '\n' ' ' | \
        sed -r 's/[ ]+/ /g' | tr 'A-Z' 'a-z' | \
        tr -d 'aeuiovkxjzq' | \
        dd of=src_$test"_"$i.txt bs=1 count=$i skip=$offset
  done
done

# vim: set ts=2 sw=2 et:
