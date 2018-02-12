set terminal png size 800,500 enhanced font "Helvetica,16"
set output '../task-perf-results.png'

red = "#FF0000"; green = "#00FF00"; blue = "#0000FF";
set yrange [0:48]
set style data histogram
set style histogram cluster gap 1
set style fill solid
set boxwidth 0.9
set xtics format ""
set grid ytics

set title "OpenMP Performance Comparison" font "Helvetica, 18"
set label "(ICC 18.1, 2 x Intel Xeon Platinum 8160 CPU (24 cores))" at screen 0.5, 0.875 center
set ylabel "Speedup against serial version" offset 1.5,0,0 font "Helvetica,14"
set xlabel "Benchmark" offset 0,+0.5,0 font "Helvetica,14"
plot "openmp_perf_comparison.data" using 2:xtic(1) title "omp-for" linecolor rgb red, \
         '' using 3 title "omp-tasks" linecolor rgb blue, \
         '' using 4 title "omp-tasks-deps" linecolor rgb green
