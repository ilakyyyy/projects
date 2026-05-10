import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

#I know there are already-made functions in C for this -- this is just less messy with all the parallel threads running

#Initial parameters
L = 128
n_runs = 15
all_data = []

#Read the 15 archives
for i in range(n_runs):
    filename = f"results/hysteresis_L{L}_run_{i}.txt"
    df = pd.read_csv(filename, sep='\t', names=['beta', 'en', 'mag', 'en2', 'mag2'])
    all_data.append(df)

# 3. Concatenar y agrupar por beta
# Ojo: Como la histéresis va y vuelve, el mismo beta aparece dos veces. 
# Necesitamos separar la IDA de la VUELTA.
df_total = pd.concat(all_data)

# Dividimos el dataframe a la mitad (ida y vuelta)
half = len(all_data[0]) // 2
ida_list = [d.iloc[:half] for d in all_data]
vuelta_list = [d.iloc[half:] for d in all_data]

# 4. Calcular medias y desviaciones
def get_stats(list_of_dfs):
    combined = pd.concat(list_of_dfs)
    mean = combined.groupby('beta').mean().reset_index()
    std = combined.groupby('beta').std().reset_index()
    return mean, std

ida_mean, ida_std = get_stats(ida_list)
vuelta_mean, vuelta_std = get_stats(vuelta_list)

# 5. Graficar la Histéresis
plt.figure(figsize=(10, 6))

# Dibujamos la ida
plt.errorbar(ida_mean['beta'], ida_mean['mag'], yerr=ida_std['mag']/np.sqrt(n_runs), 
             label='Enfriamiento (Ida)', fmt='-o', capsize=3)

# Dibujamos la vuelta
plt.errorbar(vuelta_mean['beta'], vuelta_mean['mag'], yerr=vuelta_std['mag']/np.sqrt(n_runs), 
             label='Calentamiento (Vuelta)', fmt='-o', capsize=3)

plt.title(f"Ciclo de Histéresis Modelo de Ising (L={L}, n=15)")
plt.xlabel(r"$\beta$ (1/T)")
plt.ylabel("Magnetización <|M|>")
plt.legend()
plt.grid(True, alpha=0.3)
plt.savefig(f"plots/hysteresis_L{L}.png", dpi=300, bbox_inches='tight')