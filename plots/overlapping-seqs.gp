# gnuplot ip/xm_prng/doc/quality/overlapping-seqs.gp
set term pdfcairo size 5in,3in
set output "overlapping-seqs.pdf"
set xrange [1:32]
set log y
set xlabel "Days of runtime"
set ylabel "Probability"
set key box opaque
set border black
set style data linespoint
set style line 1 lt 2 lc rgb "grey" lw 1
set style line 2 lt 1 lc 1 lw 1
set style line 3 lt 1 lc 2 lw 1
set style line 4 lt 1 lc 3 lw 1
plot "overlapping-seqs.dat" using 1:2 title "16" ls 1, \
     "overlapping-seqs.dat" using 1:3 title "1K" ls 2, \
     "overlapping-seqs.dat" using 1:4 title "65K" ls 3
