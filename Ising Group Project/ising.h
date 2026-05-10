#include <math.h>
#include <stdio.h>
#include <stdlib.h>

//**********PARISI RAPUANO*************
#define NormRANu (2.3283063671E-10F)

extern unsigned int irr[256];
extern unsigned int ir1;
extern unsigned char ind_ran,ig1,ig2,ig3;

extern float Random(void);
extern void ini_ran(int SEMILLA);
//************************************

typedef struct {
    int L;                      // Size of the lattice
    unsigned int seed;          // Random seed for reproducibility
    int configuration_flag;     // Flag to indicate the type of configuration to generate
    int n_measurements;         // Number of measurements to perform
    int n_metropolis_steps;     // Number of Metropolis steps between measurements
    int n_thermalization_steps; // Number of Metropolis steps for thermalization
    double beta_initial;        // Initial inverse temperature (1/T)
    double beta_final;          // Final inverse temperature (1/T)
    double beta_step;           // Step size for beta
} Parameters;

typedef struct {
    int L;          // Size of the lattice
    char* s;        // Spin configuration (1D array representing 2D lattice)
    int* xp;        // Direction vector for x (right)
    int* yp;        // Direction vector for y (up)
    int* xm;        // Direction vector for x (left)
    int* ym;        // Direction vector for y (down)
    double* prob;   // Probability array for Metropolis updates
} Lattice;

/**
 * Function prototypes
 */

// Function prototypes for analysis.c
void med_var(double* data, int n_data, double* mean, double* variance);
void histogram(double* data, int n_data, double* hist, int n_bins, double* delta, double* min, double* max);

// Function prototypes for io.c
void save_configuration(const char* filename, char* s, int L);
void save_histogram(const char* filename, double* hist, int n_bins, double delta, double min);

// Function prototypes for memory.c
void initialize_lattice_and_measurements(Parameters parameters, Lattice* lattice, double** energy, double** magnetization, double** abs_magnetization);
void free_lattice_and_measurements(Lattice* lattice, double* energy, double* magnetization, double* abs_magnetization);

// Function prototypes for metropolis.c
void measure_observables(Parameters* parameters, Lattice* lattice, double* energy, double* magnetization);
void metropolis_update(Lattice* lattice);
void initialize_probability_array(Lattice* lattice, double beta);

// Function prototypes for observables.c
void build_direction_vectors(int *xp, int *yp, int *xm, int *ym, int L);
double calculate_energy(char* s, int* xp, int* yp, int L);
double calculate_magnetization(char* s, int L);

// Function prototypes for utils.c
void generate_random_configuration(char *s, int L);
void generate_all_up_configuration(char *s, int L);
void generate_all_down_configuration(char *s, int L);
void generate_chess_board_configuration(char *s, int L);
void generate_configuration(Parameters* parameters, char *s);

// Functions from auxiliaries .c's
void histeresis(Lattice *L, Parameters P);

// Function prototypes for ising.c
void run_simulation(double beta, int L, int n_measurements, int n_metropolis_steps, int n_thermalization_steps, double* out_energy, double* out_specific_heat, double* out_magnetization, double* out_susceptibility);
void HisteresisMod(Lattice *L, Parameters P, double *energy, double *magnetization, char *filename);