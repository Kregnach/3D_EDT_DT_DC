#!/bin/sh

for k in -10 ; do
for l in -10; do
for c in -10; do
for v in 1000; do
for N in 1; do
#name=conf-${k}-${d}-${T}-${V}k-${to}.dat
#if [ ! -e ${name} ]; then
#    continue;
#fi

cat > Cfg-$k-$l-$c-${v}-$N.txt << EOF
seed 						-1
kappa0						$k
kappa3						2.5984
betac						$c
betal						$l
sigma3						0.05
m_bistellar_flip				0
m_vertextree					1
m_dualtree					1
m_quadrangular_pillow				1
m_triangular_pillow				0

tuningsweeps					10
measurementsweeps				0
thermalizationsweeps				1000
ksteps						1

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
adjacency_matrix_version			0

verbose						0

EOF

done;
done;
done;
done;
done;
#done;
