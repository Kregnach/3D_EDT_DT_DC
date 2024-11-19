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
kappa0						$k
kappa3						3.6984
betac						$c
betal						$l
sigma3						0.05
m_bistellar_flip				1
m_vertextree					1
m_dualtree					1
m_quadrangular_pillow				1
m_triangular_pillow				1

tuningsweeps					10
measurementsweeps				10
thermalizationsweeps				10
ksteps						10

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
