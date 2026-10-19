#!/bin/bash
if [[ $1 == "" ]]; then
  echo "expected path as first argument"
  exit 1
fi

for f in $(find $1 -type f); do
  tr -s ' ' '\n' < "$f" | sort -u
done | sort | uniq -c
