#!/bin/sh

for l in $(seq 4 2 10); do
for k in 2 ; do
for c in -1; do
for v in 4; do

name=conf-${k}-${d}-${T}-${V}k-sphere.dat
if [ ! -e ${name} ]; then
    continue;
fi

cat > Cfg-$k-$l-$c-${v}k.txt << EOF
seed 					-1
k0						$k
k3						-1
betac					$c
betal					$l
sigma3					0.1
prob23					2
probtree				4

measurementsweeps		5000
thermalizationsweeps	1000
ksteps					40

noselfadjacency			1
nodegeneracy			1
dualspanningtree		1
spanningtree			1
middlegraph				1

volume					${v}000

input					0
infile					input.txt
outfile					output.txt
fileID					$k-$l-$c-${v}k
EOF

cat > run-${k}-${l}-${c}-${v}k.sh << EOF

#!/bin/sh
#PBS -N something
#PBS -q vlong
#PBS -l walltime=4000:00:00
#PBS -l cput=4000:00:00
#PBS -m abe
#PBS -M nemeth.daniel.1992@gmail.com

#trap "Trap3" SIGNT SIGTERM
cd /home/kregnach/sphere/

./DT  Cfg-$k-$l-$c-${v}k.txt >> Out-$k-$l-$c-${v}k.txt 2> Err-$k-$l-$c-${v}k-sphere.txt
EOF


cat > run.x << EOF
for l in \$(seq 4 2 10); do
for k in 2 ; do
for c in -1; do
for v in 4; do

name=\${k}-\${l}-\${c}-\${v}k

if [ ! -e run-\${name}.sh ]; then
    print \${name}
continue
fi

qsub run-\${name}.sh -N \${name}

done;
done;
done;
done;

EOF
chmod u+x run.x

#chmod u+x run-${k}-${d}-${T}-${V}k-torus.sh
done;
done;
done;
done;
