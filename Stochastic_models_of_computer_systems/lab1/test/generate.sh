#!/bin/bash

i=2500
offset=10000

for i in 1500 2500 4000 5000; do
  python gen_random.py $i > random_$i.txt

  for src in ../data/war_and_peace.txt ../data/crime_and_punishment.txt; do
    cat $src | sed 's/[^A-Za-z ]/ /g' | tr '\n' ' ' | \
        sed -r 's/[ ]+/ /g' | tr 'A-Z' 'a-z' | \
        dd of=src_$src"_"$i.txt bs=1 count=$i skip=$offset
  done
done

# vim: set ts=2 sw=2 et:
