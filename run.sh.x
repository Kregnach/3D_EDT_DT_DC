#!/bin/sh

for c in -10; do
#$(seq 20.00 10.00 100.00) ; do
for l in -10 ; do
for k in -10; do
for v in 1000; do
for N in 1; do
#./DT --middlegraph --verbose -k$k -C$c -v$v -L$l -s0.1 -m60000 -h100 -f10 -o output-$k-$l-$c.txt &

#for k in $(seq 0.0 0.4 6.0) ; do
#tunesmall #
./DT Cfg-$k-$l-$c-${v}-$N.txt &

done;
done;
done;
done;
done;

exit;

