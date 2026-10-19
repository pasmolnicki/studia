if [[ $1 == "" ]]; then
  echo "expected path as first argument"
  exit 1
fi

for f in $(find $1 -type f); do
  cat $f
done | tr -s ' ' '\n' | sort | uniq -c

# find $1 -type f -exec cat {} + | tr -s ' ' '\n' | sort | uniq -c

