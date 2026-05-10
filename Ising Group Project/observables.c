#include "ising.h"

/**
 * Builds the direction vectors for the lattice, which are used to determine 
 * the neighboring spins in the x and y directions.
 */
void build_direction_vectors(int *xp, int *yp, int *xm, int *ym, int L) {
    for (int i=0;i<(L-1);i++){
        xp[i]=1;
    }
    xp[L-1] = -(L-1);
    for (int i=0;i<(L-1);i++){
        yp[i]=L;
    }
    yp[L-1] = -L*(L-1);
    for (int i=0;i<L;i++){
        xm[i]=-1;
    }
    xm[0] = (L-1);
    for (int i=0;i<L;i++){
        ym[i]=-L;
    }
    ym[0] = L*(L-1);
}

/**
 * Calculates the energy of a given spin configuration 's' on the lattice using
 * the equation of the ising model
 */
double calculate_energy(char* s, int* xp, int* yp, int L) {
    double E = 0;
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < L; j++) {
            E += (s[L*i + j]*(s[L*i + j + xp[j]]+s[L*i + j + yp[i]]));
        }
    }
    return -E/(2*L*L); //valor intensivo
}

/**
 * Calculates the magnetization of a given spin configuration 's' on the lattice 
 * using equation 9.2.
 */
double calculate_magnetization(char* s, int L) {
    double m=0;
    double magnetization;

    for (int i = 0; i < L*L; i++){
        m+=s[i];
    }
    magnetization = 1.0/(L*L) * m; 

    return magnetization;
}

#ifdef DEBUG_OBSERVABLES
void print_debug_observables(Parameters parameters, Lattice lattice, double energy, double magnetization) {
    printf("Debugging observables.c...\n");

    printf("For the configuration:\n");
    for (int i = 0; i < parameters.L; i++) {
        for (int j = 0; j < parameters.L; j++) {
            printf("%2d ", lattice.s[i * parameters.L + j]);
        }
        printf("\n");
    }
    printf("\nEnergy: %f\n", energy);
    printf("Magnetization: %f\n", magnetization);
    printf("\nFinished calculating observables.\n\n\n");

}

int main() {
    // Declare and initialize parameters and lattice
    Parameters parameters = { .L = 64, .seed = 42, .configuration_flag = 0 };
    Lattice lattice;

    double energy, magnetization;

    ini_ran(parameters.seed);
    initialize_lattice_and_measurements(parameters, &lattice, NULL, NULL, NULL);

    // Generate the configuration based on the parameters
    generate_configuration(&parameters, lattice.s);
    build_direction_vectors(lattice.xp, lattice.yp, lattice.xm, lattice.ym, lattice.L);

    // Calculate and print the energy and magnetization of the configuration
    energy = calculate_energy(lattice.s, lattice.xp, lattice.yp, lattice.L);
    magnetization = calculate_magnetization(lattice.s, lattice.L);

    // Print the calculated observables for debugging purposes
    print_debug_observables(parameters, lattice, energy, magnetization);

    // Release allocated memory
    free_lattice_and_measurements(&lattice, NULL, NULL, NULL);


    return 0;
}
#endif