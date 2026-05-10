#include "ising.h"

/**
 * Week 2: measures the energy and magnetization of the system n_measurements times
 * while re-generating the configuration one time after each measurement.
 * 
 * Week 3: measures the energy and magnetization of the system n_measurements times,
 * while performing n_metropolis_steps between each measurement.
 * 
 * The results are stored in the arrays 'energy' and 'magnetization'.
 */
void measure_observables(Parameters* parameters, Lattice* lattice, double* energy, double* magnetization) {
   int i, j;
    for (i = 0; i < parameters->n_measurements; i++) {
        energy[i] = calculate_energy(lattice->s, lattice->xp, lattice->yp, lattice->L);
        magnetization[i] = calculate_magnetization(lattice->s, lattice->L);
        
        for (j = 0; j < parameters->n_metropolis_steps; j++) {
            metropolis_update(lattice);
        }
    }
}

/**
 * Initialize the probability array for the Metropolis updates based on the temperature and 
 * the possible energy changes, following equation 11.6. This function should be called after 
 * updating the temperature and before performing the Metropolis updates. 
 */
void initialize_probability_array(Lattice* lattice, double beta) {
    (*lattice).prob[0]=exp(-beta*(-8));
    (*lattice).prob[1]=exp(-beta*(-4));
    (*lattice).prob[2]=1.0;
    (*lattice).prob[3]=exp(-beta*(4));
    (*lattice).prob[4]=exp(-beta*(8));
}

/**
 * Perform the Metropolis algorithm to update the spin configuration of the lattice.
 */
void metropolis_update(Lattice* lattice) {
    int n,y,x;
    int Ind;
    n = 0;
    for (y=0;   y < (*lattice).L;   y++){
        for(x=0;    x < (*lattice).L ;    x++){
            Ind=
            (*lattice).s[n] * (
                (*lattice).s[n+(*lattice).xp[x]] + 
                (*lattice).s[n+(*lattice).yp[y]] + 
                (*lattice).s[n+(*lattice).xm[x]] + 
                (*lattice).s[n+(*lattice).ym[y]]
            ) / 2 + 2;
            if(Random() < (*lattice).prob[Ind]) {
                (*lattice).s[n]=-(*lattice).s[n];
            }
            n++;
        }

    }
}

#ifdef DEBUG_METROPOLIS
void print_debug_metropolis(Parameters parameters, double beta, double* energy, double* magnetization, double* abs_magnetization) {
    double energy_mean, energy_variance;
    double magnetization_mean, magnetization_variance;
    double abs_magnetization_mean, abs_magnetization_variance;

    printf("Debugging Metropolis:\n");

    med_var(energy, parameters.n_measurements, &energy_mean, &energy_variance);
    med_var(magnetization, parameters.n_measurements, &magnetization_mean, &magnetization_variance);
    med_var(abs_magnetization, parameters.n_measurements, &abs_magnetization_mean, &abs_magnetization_variance);
    
    printf("Measurements for beta = %lf:\n\n", beta);
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
    /*****************/
    /* Initial Setup */
    /*****************/

    Parameters parameters = { 
        .L = 16, 
        .seed = 42, 
        .configuration_flag = 0, 
        .n_measurements = 100, 
        .n_metropolis_steps = 100, 
        .n_thermalization_steps = 100
    };

    Lattice lattice;

    double *energy, *magnetization, *abs_magnetization;

    ini_ran(parameters.seed);
    initialize_lattice_and_measurements(parameters, &lattice, &energy, &magnetization, &abs_magnetization);

    // Generate the configuration based on the parameters
    generate_configuration(&parameters, lattice.s);
    build_direction_vectors(lattice.xp, lattice.yp, lattice.xm, lattice.ym, lattice.L);

    /**************/
    /* Main logic */
    /**************/

    // Set the temperature of the system
    double beta = 2; // Inverse temperature (1/T)
    initialize_probability_array(&lattice, beta);

    // Measure n_measurements times the energy and magnetization of the system after thermalization
    for (int i = 0; i < parameters.n_thermalization_steps; i++) {
        metropolis_update(&lattice);
    }
    measure_observables(&parameters, &lattice, energy, magnetization);

    // Get absolute value of magnetization
    for (int i = 0; i < parameters.n_measurements; i++) {
        abs_magnetization[i] = fabs(magnetization[i]);
    }

    // Compute and print the mean and variance of the observables
    print_debug_metropolis(parameters, beta, energy, magnetization, abs_magnetization);
    print_analysis_histogram(energy, magnetization, parameters.n_measurements,parameters.L);

    /************/
    /* Clean up */
    /************/

    // Release allocated memory
    free_lattice_and_measurements(&lattice, energy, magnetization, abs_magnetization);

    return 0;
}
#endif