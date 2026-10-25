echo "Running julia files..."
for F in $(find . -name "*.jl" | sort)
do
  echo ""
  echo "-------------------"
  echo "Results: $F"
  echo ""
  julia $F
done > jl_output

echo "Running c files..."
for F in $(find . -name "*.c")
do
  echo ""
  echo "-------------------"
  echo "Results: $F"
  echo ""
  FILE_NAME="${F##*/}"
  BASE_NAME="${FILE_NAME%.*}"
  gcc -o $BASE_NAME $F && "./${BASE_NAME}"
done > c_output
