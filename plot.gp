#!/usr/bin/env gnuplot5

#name = "2.000000_0.500000_1.000000_1.000000_0.500000_0.400000_0.200000_0.100000"
scoreFile = name.".dat"
generationFile = name."_generation.dat"
tileProbabilityFile = name."_tileprobability.dat"
mutationRatesFile = name."_mutationrates.dat"
statsFile = name."_stats.dat"

set term GPVAL_TERM 0

set xlabel "generation"
set ylabel "average score"
unset key

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

stats generationFile u 2 name 'y' nooutput;

set yrange [y_min:y_max]

p generationFile u 1:2,\
8*2**9 w l ls 1,\
9*2**10 w l ls 1,\
10*2**11 w l ls 1,\
11*2**12 w l ls 1

set term GPVAL_TERM 1

set yrange [*:*]
set ylabel "success rate"
set key top left

p tileProbabilityFile u 1:10 title "256" w l ls 11,\
tileProbabilityFile u 1:11 title "512" w l ls 12,\
tileProbabilityFile u 1:12 title "1024" w l ls 13,\
tileProbabilityFile u 1:13 title "2048" w l ls 14


set term GPVAL_TERM 2

set yrange [*:*]
set ylabel "rate"
set key top left

p mutationRatesFile u 1:2:3 title "weight mutate" w errorbars ls 11,\
mutationRatesFile u 1:4:5 title "link mutate" w errorbars ls 12,\
mutationRatesFile u 1:6:7 title "bias mutate" w errorbars ls 13,\
mutationRatesFile u 1:8:9 title "node mutate" w errorbars ls 14,\
mutationRatesFile u 1:10:11 title "enable mutate" w errorbars ls 21,\
mutationRatesFile u 1:12:13 title "disable mutate" w errorbars ls 22,\
mutationRatesFile u 1:14:15 title "transfer mutate" w errorbars ls 23,\
mutationRatesFile u 1:16:17 title "delete mutate" w errorbars ls 24,\
mutationRatesFile u 1:18:19 title "step size mutate" w errorbars ls 31

set term GPVAL_TERM 3

binwidth=32
bin(x,width)=width*floor(x/width)
plot scoreFile using (bin($1,binwidth)):($0 < 10*1500 && $0 >= 9*1500 ? 1.0 : 0.0) smooth freq with boxes,\
scoreFile using (bin($1,binwidth)):(($0 < 50*1500 && $0 >= 49*1500)? 1.0 : 0.0) smooth freq with boxes,\
scoreFile using (bin($1,binwidth)):(($0 < 100*1500 && $0 >= 99*1500)? 1.0 : 0.0) smooth freq with boxes


set term GPVAL_TERM 4

set y2tics
set y2label "genes"
set ylabel "neurons"

plot statsFile using 1:2 title "total neurons" w l ls 11,\
statsFile using 1:3 title "active neurons" w l ls 12,\
statsFile using 1:4 title "mutable neurons" w l ls 13,\
statsFile using 1:5 title "total genes" axes x1y2 w l ls 21,\
statsFile using 1:6 title "enabled genes" axes x1y2 w l ls 22,\
statsFile using 1:7 title "disabled genes" axes x1y2 w l ls 23
