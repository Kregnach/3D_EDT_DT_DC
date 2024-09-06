#!/bin/sh

for k in 2 ; do
for l in 0; do
for c in 0; do
for v in 1000; do
for N in 1; do
#name=conf-${k}-${d}-${T}-${V}k-${to}.dat
#if [ ! -e ${name} ]; then
#    continue;
#fi

cat > Cfg-$k-$l-$c-${v}-$N.txt << EOF
seed 						-1
k0						$k
k3						3.6984
betac						$c
betal						$l
sigma3						0.05
prob23						1
probtree					1
probe02						1
probv02						1

tuningsweeps					100
measurementsweeps				0
thermalizationsweeps				100000
ksteps						4

allowselfadjacency				0
allowdegeneracy					0
dualspanningtree				1
spanningtree					1
middlegraph					1

volume						${v}
input						0
infile						conf-$k-$l-$c-${v}-$N.DT
outfile						conf-$k-$l-$c-${v}-$N.DT
fileID						$k-$l-$c-${v}-$N
number						$N
printfreq					100

measuresimplexdata				1 
measureadjacency				0
measurehistograms				0
adjacency_matrix_version		0

EOF

done;
done;
done;
done;
done;
#done;
