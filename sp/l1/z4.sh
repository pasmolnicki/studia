#!/bin/bash
if [[ $1 == "" ]]; then
  echo "expected path as first argument"
  exit 1
fi

words=$(for f in $(find $1 -type f); do
  tr -s ' ' '\n' < "$f" | sort -u
done | sort | uniq)

for w in $words; do
  echo $w
  grep -rnw "$w" $1
done
