# Output settings
set terminal pngcairo size 1400,800 enhanced font "Arial,12"
set output "SERIACONVENIENTEPONERUNNOMBREQUETENGASENTIDO.png"

# --- Styling ---
set xlabel "Token"
set ylabel "Value"

set grid ytics lc rgb "#dddddd"
set border linewidth 1.5
set tics out
set xtics rotate by 45 right font ",10"

set style fill solid 0.85 border rgb "#2c3e50"
set boxwidth 0.8

unset key

set style data histograms
set style histogram cluster gap 1

# --- Plot ---
set yrange[0:1]

plot "../results/AQUIHABRAQUEPONERELNOMBREDELARCHIVO.COMOHACEMOSENTOOOODASLASPRACTICAS" using 2:xtic(3) with boxes lc rgb "#3498db"
