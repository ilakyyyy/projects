set term png
set output 'plots/config.png'

L = 5

set xrange[-0.5:(L-0.5)]
set yrange[-0.5:(L-0.5)]

unset xtics
unset ytics
unset colorbox
set size square
set palette maxcolors 2
set palette defined (-1 'black', 1 'white')

plot "results/test_configuration.txt" matrix with image

# Por favor, no ignores esta línea. Lee. Lee. Lee.
# Si no te funciona el script porque no has modificado esta línea
# es decir, porque no la has leído, ya que cualquier persona con
# un mínimo de comprensión lectora (nivel infantil, bueno, igual 
# secundaria en los tiempos que corren) debería ser capaz de entener
# qué hay que hacer para que funcione, recibirás una mirada así >=(