#!/usr/bin/env gnuplot5

#name = "2.000000_0.500000_1.000000_1.000000_0.500000_0.400000_0.200000_0.100000"
scoreFile = name.".dat"
generationFile = name."_generation.dat"
tileProbabilityFile = name."_tileprobability.dat"
bestTileProbabilityFile = name."_bestGenomeTileprobability.dat"
mutationRatesFile = name."_mutationrates.dat"
statsFile = name."_stats.dat"

set style line 1 lw 1 dt '.' lc rgb "gray"
set style line 11 lw 1 dt '_' lc rgb "#A00000"
set style line 12 lw 1 dt '_' lc rgb "#FF0000"
set style line 13 lw 1 dt '_' lc rgb "#FF8080"
set style line 14 lw 1 dt '_' lc rgb "#FFC0C0"
set style line 21 lw 1 dt '_' lc rgb "#0000A0"
set style line 22 lw 1 dt '_' lc rgb "#0000FF"
set style line 23 lw 1 dt '_' lc rgb "#8080FF"
set style line 24 lw 1 dt '_' lc rgb "#C0C0FF"
set style line 31 lw 1 dt '_' lc rgb "#00A000"
set style line 41 lw 1 dt '_' lc rgb "#00FF00"

log2(x) = log(x)/log(2)

set term GPVAL_TERM 0

set title "Fitness"

set ytics nomirror
set y2tics
set xlabel "generation"
set ylabel "score"
set y2label "precision"
set key top left

stats generationFile u 4 name 'min' nooutput;
stats generationFile u 5 name 'max' nooutput;

set yrange [min_min:(max_max+0.1)]

p generationFile u 1:2 title "score" w l ls 22,\
generationFile u 1:4 title "min" w l ls 12,\
generationFile u 1:5 title "max" w l ls 41,\
generationFile u 1:3 title "precision" axes x1y2 w l ls 1,\
log2(8*2**9) notitle w l ls 1,\
log2(9*2**10) notitle w l ls 1,\
log2(10*2**11) notitle w l ls 1,\
log2(11*2**12) notitle w l ls 1

set term GPVAL_TERM 1

set title "generation success"

set ytics mirror
unset y2tics
unset y2label

set yrange [-0.01:1.01]
set xrange [*:*]
set ylabel "success rate"
set key top left

p tileProbabilityFile u 1:8 title "64" w l ls 11,\
tileProbabilityFile u 1:9 title "128" w l ls 12,\
tileProbabilityFile u 1:10 title "256" w l ls 13,\
tileProbabilityFile u 1:11 title "512" w l ls 14,\
tileProbabilityFile u 1:12 title "1024" w l ls 21,\
tileProbabilityFile u 1:13 title "2048" w l ls 22

set term GPVAL_TERM 2

set title "best genome success"

set ytics mirror
unset y2tics
unset y2label

set yrange [-0.01:1.01]
set xrange [*:*]
set ylabel "success rate"
set key top left

p bestTileProbabilityFile u 1:8 title "64" w l ls 11,\
bestTileProbabilityFile u 1:9 title "128" w l ls 12,\
bestTileProbabilityFile u 1:10 title "256" w l ls 13,\
bestTileProbabilityFile u 1:11 title "512" w l ls 14,\
bestTileProbabilityFile u 1:12 title "1024" w l ls 21,\
bestTileProbabilityFile u 1:13 title "2048" w l ls 22


set term GPVAL_TERM 3

set title "mean mutation rates"

set yrange [*:*]
set ylabel "rate"
set key top left

p mutationRatesFile u 1:2 title "weight mutate" ls 11,\
mutationRatesFile u 1:3 title "link mutate" ls 12,\
mutationRatesFile u 1:4 title "bias mutate" ls 13,\
mutationRatesFile u 1:5 title "node mutate" ls 14,\
mutationRatesFile u 1:6 title "enable mutate" ls 21,\
mutationRatesFile u 1:7title "disable mutate" ls 22,\
mutationRatesFile u 1:8 title "transfer mutate" ls 23,\
mutationRatesFile u 1:9 title "delete mutate" ls 24,\
mutationRatesFile u 1:10 title "step size mutate" ls 31

set term GPVAL_TERM 4

#binwidth=32
#bin(x,width)=width*floor(x/width)
#plot scoreFile using (bin($1,binwidth)):($0 < 10*1500 && $0 >= 9*1500 ? 1.0 : 0.0) smooth freq with boxes,\
#scoreFile using (bin($1,binwidth)):(($0 < 50*1500 && $0 >= 49*1500)? 1.0 : 0.0) smooth freq with boxes,\
#scoreFile using (bin($1,binwidth)):(($0 < 100*1500 && $0 >= 99*1500)? 1.0 : 0.0) smooth freq with boxes

set term GPVAL_TERM 5

set title "number of species"

stats statsFile u 8 name 'species' nooutput;
set ylabel "species"
set yrange [species_min-1:species_max+1]
plot statsFile using 1:8 notitle w l ls 11

set term GPVAL_TERM 6

set title "size statistics"

set ytics nomirror
set y2tics
set y2label "genes"
set ylabel "neurons"
set yrange [*:*]

plot statsFile using 1:2 title "total neurons" w l ls 11,\
statsFile using 1:3 title "active neurons" w l ls 12,\
statsFile using 1:4 title "mutable neurons" w l ls 13,\
statsFile using 1:5 title "total genes" axes x1y2 w l ls 21,\
statsFile using 1:6 title "enabled genes" axes x1y2 w l ls 22,\
statsFile using 1:7 title "disabled genes" axes x1y2 w l ls 23
