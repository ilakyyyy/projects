#include "ising.h"

/**
 * hace la simulación con beta, L y los pasos (de medidas, metropolis y termalización)
 * devuelve energía, calor específico, magnetización y susceptibilidad
 */
void run_simulation(double beta, int L, int n_measurements, int n_metropolis_steps, int n_thermalization_steps, 
                    double* out_energy, double* out_specific_heat, double* out_magnetization, double* out_susceptibility) {
    Parameters parameters = { 
        .L = L, 
        .seed = 42, 
        .configuration_flag = 0, 
        .n_measurements = n_measurements, 
        .n_metropolis_steps = n_metropolis_steps, 
        .n_thermalization_steps = n_thermalization_steps
    };

    ini_ran(parameters.seed);

    Lattice lattice;
    double *energy, *magnetization, *abs_magnetization;

    initialize_lattice_and_measurements(parameters, &lattice, &energy, &magnetization, &abs_magnetization);
    generate_configuration(&parameters, lattice.s);
    build_direction_vectors(lattice.xp, lattice.yp, lattice.xm, lattice.ym, lattice.L);

    initialize_probability_array(&lattice, beta);

    for (int i = 0; i < parameters.n_thermalization_steps; i++) {
        metropolis_update(&lattice);
    }

    measure_observables(&parameters, &lattice, energy, magnetization);

    for (int i = 0; i < parameters.n_measurements; i++) {
        abs_magnetization[i] = fabs(magnetization[i]);
    }

    double energy_mean, energy_variance, abs_mag_mean, abs_mag_variance;
    med_var(energy, parameters.n_measurements, &energy_mean, &energy_variance);
    med_var(abs_magnetization, parameters.n_measurements, &abs_mag_mean, &abs_mag_variance);

    *out_energy = energy_mean;
    *out_specific_heat = beta * beta * L * L * energy_variance; 
    *out_magnetization = abs_mag_mean;
    *out_susceptibility = beta * L * L * abs_mag_variance;

    free_lattice_and_measurements(&lattice, energy, magnetization, abs_magnetization);
}

int main() {
    int Llist[] = {64,80,90};
    for (int sim = 0; sim < 3; sim ++) {
        double beta = 1;
        int L = Llist[sim];
        int n_measurements = 100;
        int n_metropolis_steps = 100;
        int n_thermalization_steps = 10*L*L;

        double mean_energy, specific_heat, mean_magnetization, susceptibility;

        char filename[256];
        sprintf(filename, "results/simulations_L%03d.txt", L);
        
        FILE *f=fopen(filename, "w");

        if(f==NULL){
            printf("no se ha podido abrir el fichero");
            return 1; 
        }

        fprintf(f,"%8s   %12s   %13s   %18s   %14s\n", 
               "beta", "energy", "spec-heat", "abs-magnet", "suscept");

        for (int i = 0; i < 100; i++) {
            beta = beta - .01;
            // bastante arbitrario haber elegido .8 para el factor par asacar la siguiente beta
            // ahora estoy haciendo barrido lineal
            run_simulation(beta, L, n_measurements, n_metropolis_steps, n_thermalization_steps, 
                       &mean_energy, &specific_heat, &mean_magnetization, &susceptibility);
            
            fprintf(f,"%8.4f   %12.6f   %13.6f   %18.6f   %14.6f\n", 
                   beta, mean_energy, specific_heat, mean_magnetization, susceptibility);
        }

        // fprintf(f,"\nparams\n\tL             = %d\n\tn_measurs     = %d\n\tn_metro_steps = %d\n\tn_therm_steps = %d\n", L, n_measurements, n_metropolis_steps, n_thermalization_steps);

        fclose(f);
    }
    return 0;
}
// int main(){
// Parameters parameters = { //varien todos los parametros como les apetezca
//         .L = 8, 
//         .seed = 0, 
//         .configuration_flag = 0, 
//         .n_measurements = 10, 
//         .n_metropolis_steps =3,
//         .n_thermalization_steps =2,
//         .beta_initial=0,
//         .beta_final=1.5,
//         .beta_step=0.05
//     };

//     Lattice lattice;

//     double *energy, *magnetization,*absmagnetization;
//     ini_ran(parameters.seed);
//     initialize_lattice_and_measurements(parameters, &lattice, &energy, &magnetization,&absmagnetization);
//     build_direction_vectors(lattice.xp, lattice.yp, lattice.xm, lattice.ym, lattice.L);
//     Histeresis(&lattice,parameters,energy,magnetization);
//     free_lattice_and_measurements(&lattice, energy, magnetization,absmagnetization);
//     return 0;
// }
