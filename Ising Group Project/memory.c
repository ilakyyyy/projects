#include "ising.h"

/**
 * Allocates the necessary memory for the lattice and the measurement arrays
 */
void initialize_lattice_and_measurements(Parameters parameters, Lattice* lattice, double** energy, double** magnetization, double** abs_magnetization) {
    lattice->L = parameters.L;
    lattice->s = (char*)malloc(lattice->L * lattice->L * sizeof(char));
    lattice->xp = (int*)malloc(lattice->L * sizeof(int));
    lattice->yp = (int*)malloc(lattice->L * sizeof(int));
    lattice->xm = (int*)malloc(lattice->L * sizeof(int));
    lattice->ym = (int*)malloc(lattice->L * sizeof(int));
    lattice->prob = (double*)malloc(5 * sizeof(double));

    if (energy != NULL) {
        *energy = (double*)malloc(parameters.n_measurements * sizeof(double));
    }

    if (magnetization != NULL) {
        *magnetization = (double*)malloc(parameters.n_measurements * sizeof(double));
    }

    if (abs_magnetization != NULL) {
        *abs_magnetization = (double*)malloc(parameters.n_measurements * sizeof(double));
    }
}

/**
 * Frees the allocated memory for the lattice and the measurement arrays
 */
void free_lattice_and_measurements(Lattice* lattice, double* energy, double* magnetization, double* abs_magnetization) {
     free(energy);
     free(magnetization);
     free(abs_magnetization);
     free(lattice->s);
     free(lattice->xp);
     free(lattice->yp);
     free(lattice->xm);
     free(lattice->ym);
     free(lattice->prob);
}