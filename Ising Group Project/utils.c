#include "ising.h"

/**
 * Generates a random configuration of spins for the Ising model.
 * Each spin can be either +1 or -1, and the configuration is stored in the array 's'.
 */
void generate_random_configuration(char *s, int L) {
    int i;
    for(i=0; i<L*L; i++){
        if (Random() < 0.5)
            s[i]= +1;
        else
            s[i]= -1;
    }
}

/**
 * Generates an all-up configuration of spins for the Ising model.
 */
void generate_all_up_configuration(char *s, int L) {
    int i;
    for(i=0; i<L*L; i++){
        s[i]= +1;
    }
}

/**
 * Generates an all-down configuration of spins for the Ising model.
 */
void generate_all_down_configuration(char *s, int L) {
    int i;
    for(i=0; i<L*L; i++){
        s[i]= -1;
    }
}

/**
 * Generates a chessboard configuration of spins for the Ising model.
 * Spins alternate between +1 and -1 in a checkerboard pattern.
 */
void generate_chess_board_configuration(char *s, int L) {
    int i;
    for(i=0; i<L*L; i++){
        if(i%2 == 0) 
            s[i]= +1;
        else 
            s[i]= -1;

    }
}

/**
 * Generates a configuration of spins based on the specified configuration flag in the Parameters struct.
 * The generated configuration is stored in the array 's'.
 * 
 * Dictionary: Inside parameter configuration flag:
 *     "-1"=> All down configuration (default)
 *     "0"=> Random configuration
 *     "1"=> All up configuration
 *     "2"=> Chessboard configuration
 * Nota de desarrollador: Revisar cómo hacer uso de la semilla
 */
void generate_configuration(Parameters* parameters, char *s) {
    if (parameters->configuration_flag==0) generate_random_configuration(s,parameters->L);
    else if(parameters->configuration_flag==-1) generate_all_down_configuration(s,parameters->L);
    else if(parameters->configuration_flag==1) generate_all_up_configuration(s,parameters->L);
    else if(parameters->configuration_flag==2) generate_chess_board_configuration(s,parameters->L);
    else{
        printf("Error ocurred, configuration flag '%d' not valid, defaulted to down configuration", parameters->L);
        generate_all_down_configuration(s,parameters->L);

    }
}

#ifdef DEBUG_UTILS
void print_debug_utils(Parameters parameters, Lattice lattice) {
    printf("Debugging utils.c...\n");

    printf("Generated Configuration:\n\n");
    for (int i = 0; i < parameters.L; i++) {
        for (int j = 0; j < parameters.L; j++) {
            printf("%2d ", lattice.s[i * parameters.L + j]);
        }
        printf("\n");
    }
    printf("\nFinished generating configuration.\n\n\n");
}

int main() {
    // Declare and initialize parameters and lattice
    Parameters parameters = { .L = 64, .seed = 42, .configuration_flag = 2 };
    Lattice lattice;

    ini_ran(parameters.seed);
    initialize_lattice_and_measurements(parameters, &lattice, NULL, NULL, NULL);

    // Generate the configuration based on the parameters
    generate_configuration(&parameters, lattice.s);
    
    // Print the generated configuration for debugging purposes
    print_debug_utils(parameters, lattice);    
    save_configuration("results/test_configuration.txt", lattice.s, parameters.L);

    // Release allocated memory
    free_lattice_and_measurements(&lattice, NULL, NULL, NULL);

    return 0;
}
#endif