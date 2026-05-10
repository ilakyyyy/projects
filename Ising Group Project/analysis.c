#include "ising.h"

/**
 * Calculates the mean and variance of a given series of measurements.
 * The results are stored in the variables pointed to by 'mean' and 'variance'.
 */
void med_var(double* data, int n, double* mean, double* variance) {
    int i;
    double sum, residue;
    *mean=0;
    *variance=0;

    sum = 0.0;
    for (i=0; i<n; i++){
        sum += data[i];
    }
    *mean = sum/n;

    residue = 0.0;
    for(i=0; i<n; i++){
        residue += pow(data[i] - *mean, 2);
    }

    *variance = residue/(n-1);
}
       

/**
 * Calculates the normalized histogram of a given series of measurements.
 */
//Pendiente de revision
// void histogram(double* data, int n_data, double* hist, int n_bins, double* delta, double* min, double* max) {   
//     int i, m, M, intervalo;
//     //Calculamos max y min, calculando sus índices primero
//     m=0;
//     M=0;
//     for (i=1; i<n_data; i++){
//         if (data[i] > data[M])
//             M=i;
//         else if (data[i] < data[m])
//             m=i;
//     }
//     *max = data[M];
//     *min = data[m];

//     //Calculamos el histograma
//     *delta= (*max - *min)/n_bins;
    
//     //Evitamos división por cero:
//     if (*delta == 0.0) {
//         *delta = 1.0;
//     }

//     for (i=0; i<n_bins; i++)
//         hist[i] = 0.0;
//     for (i=0; i<n_data; i++){
//         intervalo = (int) ((data[i]- *min) / *delta);
//         if (intervalo == n_bins) 
//             intervalo = intervalo-1;
//         hist[intervalo]++;
//     }
    
//     //Normalizamos el histograma
//     for (i=0; i<n_bins; i++) 
//         hist[i] = hist[i]/(n_data * *delta);

// }

void minandmax(double *p, int n, double *max, double *min) {
    *max = *p;
    *min = *p;
    for (int i = 1; i < n; i++) {
        if (*(p+i) > *max) {
            *max = *(p+i);
        }
        if (*(p+i) < *min) {
            *min = *(p+i);
        }
    }
}

void histogram(double *data, int N_data, double *Hist, int N_bins, double *d, double *m, double *M) {
    minandmax(data, N_data, M, m);
    for (int i = 0; i < N_bins; i++) {
        Hist[i] = 0;
    }
    *d = (*M - *m) / N_bins;
    if (*d == 0) {
        *d = 0.001;
    }
    int k;
    for (int i = 0; i < N_data; i++) {
        k = (int)((*(data + i) - *m)/ *d);
        *(Hist + k) += 1;
    }
    for (int i = 0; i < N_bins; i++) {
        *(Hist + i) =  *(Hist + i) / N_data;
    }
}

#ifdef DEBUG_ANALYSIS
void print_debug_analysis(Parameters parameters, double* energy, double* magnetization, double* abs_magnetization) {
    double energy_mean, energy_variance;
    double magnetization_mean, magnetization_variance;
    double abs_magnetization_mean, abs_magnetization_variance;

    med_var(energy, parameters.n_measurements, &energy_mean, &energy_variance);
    med_var(magnetization, parameters.n_measurements, &magnetization_mean, &magnetization_variance);
    med_var(abs_magnetization, parameters.n_measurements, &abs_magnetization_mean, &abs_magnetization_variance);

    printf("Debugging Analysis:\n");

    printf("Measurements:\n\n");
    printf("\tEnergy: mean = %lf, variance = %lf\n", energy_mean, energy_variance);
    printf("\tSpecific Heat: %lf\n", 2 * parameters.L * parameters.L * energy_variance);
    printf("\tMagnetization: mean = %lf, variance = %lf\n", magnetization_mean, magnetization_variance);
    printf("\tSusceptibility: %lf\n", parameters.L * parameters.L * magnetization_variance);
    printf("\tAbs Magnetization: mean = %lf, variance = %lf\n", abs_magnetization_mean, abs_magnetization_variance);
    printf("\tAbs Susceptibility: %lf\n", parameters.L * parameters.L * abs_magnetization_variance);
    printf("\n");
}
//He añadido un entrada para generador de histograma porque pedia parameters.L sin haberla definido en la propia funcion
void print_analysis_histogram(double* energy, double* magnetization, int n_measurements,int parametersL) {
    int n_bins = 20;
    double* hist = (double*)malloc(n_bins * sizeof(double));
    double delta, min, max;
    char filename[256];

    histogram(energy, n_measurements, hist, n_bins, &delta, &min, &max);
    sprintf(filename, "results/energy_histogram_L%d.txt", parametersL);
    save_histogram(filename, hist, n_bins, delta, min);

    histogram(magnetization, n_measurements, hist, n_bins, &delta, &min, &max);
    sprintf(filename, "results/magnetization_histogram_L%d.txt", parametersL);
    save_histogram(filename, hist, n_bins, delta, min);

    free(hist);
}

int main() {
    // Declare and initialize parameters and lattice
    Parameters parameters = { 
        .L = 64, 
        .seed = 21, 
        .configuration_flag = 0, 
        .n_measurements = 10000, 
        .beta_initial =0.44, 
        .n_metropolis_steps=100,
        .n_thermalization_steps=40,
    };
    Lattice lattice;

    double *energy, *magnetization, *abs_magnetization ;

    ini_ran(parameters.seed);
    initialize_lattice_and_measurements(parameters, &lattice, &energy, &magnetization, &abs_magnetization);

    // Generate the configuration based on the parameters
    generate_configuration(&parameters, lattice.s);
    build_direction_vectors(lattice.xp, lattice.yp, lattice.xm, lattice.ym, lattice.L);
    //EXTRA; porque sino empleamos metropolis update si el, no haces nada
    initialize_probability_array(&lattice,parameters.beta_initial);
    // Measure n_measurements times the energy and magnetization of the system
    measure_observables(&parameters, &lattice, energy, magnetization);

    // Get absolute value of magnetization
    for (int i = 0; i < parameters.n_measurements; i++) {
        abs_magnetization[i] = fabs(magnetization[i]);
    }

    // Compute and print the observables
    print_debug_analysis(parameters, energy, magnetization, abs_magnetization);
    if (parameters.configuration_flag == 0) {
        print_analysis_histogram(energy, magnetization, parameters.n_measurements,parameters.L);
    }

    // Release allocated memory
    free_lattice_and_measurements(&lattice, energy, magnetization, abs_magnetization);

    return 0;
}
#endif