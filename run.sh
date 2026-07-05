#!/bin/sh
make -s 2>/dev/null || exit 1
N=${1:-1000}
HEADER="lib,trial,test_group,type,region,thread_ms,real_ms,iters,total_cpu_ms,total_real_ms,soft_faults,hard_faults"

echo "$HEADER" > nesting.csv
echo "$HEADER" > loop.csv

for t in $(seq 1 "$N"); do
  ./a.out "$t" nesting 2>/dev/null >> nesting.csv
  ./a.out "$t" loop    2>/dev/null >> loop.csv
  ./lua_perf "$t" nesting 2>/dev/null >> nesting.csv
  ./lua_perf "$t" loop    2>/dev/null >> loop.csv
  echo "trial $t done"
done
