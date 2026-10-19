if [[ $1 == "" ]]; then
  echo "expected path as first argument"
  exit 1
fi
find $1 -type f
