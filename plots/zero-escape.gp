# gnuplot overlapping-seqs.gp
set term pdfcairo dashed size 5in,3in
set xlabel "Step"
set ylabel "Fraction of set bits"
set log x
set key right bottom box opaque
set style data lines
set style line 1 lt 1 lc rgb "grey" lw 1
set style line 2 lt 2 lw 1 dt 2 lc 1
set style line 3 lt 3 lw 1 dt 1 lc 2
set style line 4 lt 4 lw 1 dt 4 lc 3
set style line 5 lt 5 lw 1 dt 5 lc 4
set output "zero-escape-1k.pdf"
plot "ze-1k-mt32.dat" using 1:2 title "mt32" ls 1, \
     "ze-1k-pcg64.dat" using 1:2 title "pcg64" ls 4, \
     "ze-1k-philox4x32.dat" using 1:2 title "philox4x32-10" ls 5, \
     "ze-1k-plus.dat" using 1:2 title "xoroshiro128plus-55-14-36" ls 2, \
     "ze-1k-aox.dat" using 1:2 title "xoroshiro128aox-55-14-36" ls 3
set output "zero-escape-1m.pdf"
plot "ze-1m-mt32.dat" using 1:2 title "mt32" ls 1, \
     "ze-1m-pcg64.dat" using 1:2 title "pcg64" ls 4, \
     "ze-1m-philox4x32.dat" using 1:2 title "philox4x32-10" ls 5, \
     "ze-1m-plus.dat" using 1:2 title "xoroshiro128plus-55-14-36" ls 2, \
     "ze-1m-aox.dat" using 1:2 title "xoroshiro128aox-55-14-36" ls 3
