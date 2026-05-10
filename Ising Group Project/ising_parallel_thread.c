#include "ising.h"
#include <omp.h>

/*                                      COMENTARIO DE ILAKY


                SOLO USA ESTE ARCHIVO SI SABES LO QUE HACE; VAS A CORRER VARIAS SIMULACIONES CON DISTINTOS BETAS EN DIFERENTES
                CORES DE TU CPU; SE PUEDE POR TANTO CALENTAR MUCHO SI LE DAS MUCHA CARGA DE TRABAJO.
                EL CÓDIGO ESTÁ OPTIMIZADO PARA QUE USE 3/4 DEL NÚMERO MÁXIMO DE CORES -- SI QUIERES USAR MÁS, CAMBIA EL SIGUIENTE PARÁMETRO;
                TEN EN CUENTA QUE HA DE ESTAR ENTRE 0 Y 1
                SI NO TE FUNCIONA POR EL .JSON, COPIA Y PEGA EN EL ISING.C ESTE PROGRAMA (NO HAGAS COMMIT DESPUÉS)
*/
/*                                      COMENTARIO DE ILAKY


                SOLO USA ESTE ARCHIVO SI SABES LO QUE HACE; VAS A CORRER VARIAS SIMULACIONES CON DISTINTOS BETAS EN DIFERENTES
                CORES DE TU CPU; SE PUEDE POR TANTO CALENTAR MUCHO SI LE DAS MUCHA CARGA DE TRABAJO.
                EL CÓDIGO ESTÁ OPTIMIZADO PARA QUE USE 3/4 DEL NÚMERO MÁXIMO DE CORES -- SI QUIERES USAR MÁS, CAMBIA EL SIGUIENTE PARÁMETRO;
                TEN EN CUENTA QUE HA DE ESTAR ENTRE 0 Y 1
                SI NO TE FUNCIONA POR EL .JSON, COPIA Y PEGA EN EL ISING.C ESTE PROGRAMA (NO HAGAS COMMIT DESPUÉS)
*/
/*                                      COMENTARIO DE ILAKY


                SOLO USA ESTE ARCHIVO SI SABES LO QUE HACE; VAS A CORRER VARIAS SIMULACIONES CON DISTINTOS BETAS EN DIFERENTES
                CORES DE TU CPU; SE PUEDE POR TANTO CALENTAR MUCHO SI LE DAS MUCHA CARGA DE TRABAJO.
                EL CÓDIGO ESTÁ OPTIMIZADO PARA QUE USE 3/4 DEL NÚMERO MÁXIMO DE CORES -- SI QUIERES USAR MÁS, CAMBIA EL SIGUIENTE PARÁMETRO;
                TEN EN CUENTA QUE HA DE ESTAR ENTRE 0 Y 1
                SI NO TE FUNCIONA POR EL .JSON, COPIA Y PEGA EN EL ISING.C ESTE PROGRAMA (NO HAGAS COMMIT DESPUÉS)
*/
/*                                      COMENTARIO DE ILAKY


                SOLO USA ESTE ARCHIVO SI SABES LO QUE HACE; VAS A CORRER VARIAS SIMULACIONES CON DISTINTOS BETAS EN DIFERENTES
                CORES DE TU CPU; SE PUEDE POR TANTO CALENTAR MUCHO SI LE DAS MUCHA CARGA DE TRABAJO.
                EL CÓDIGO ESTÁ OPTIMIZADO PARA QUE USE 3/4 DEL NÚMERO MÁXIMO DE CORES -- SI QUIERES USAR MÁS, CAMBIA EL SIGUIENTE PARÁMETRO;
                TEN EN CUENTA QUE HA DE ESTAR ENTRE 0 Y 1
                SI NO TE FUNCIONA POR EL .JSON, COPIA Y PEGA EN EL ISING.C ESTE PROGRAMA (NO HAGAS COMMIT DESPUÉS)
*/
#define MULTIPLIER 0.75
void print_analysis_histogram(double* energy, double* magnetization, int n_measurements,int parametersL) {
    int n_bins = 40;
    double* hist = (double*)calloc(n_bins, sizeof(double));
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

    // char filename[256];
    // sprintf(filename, "results/spins_gif_L%03d_B%4.3f.bin", L, beta);
    // FILE *gif_file = fopen(filename, "wb");

    /*Initial stabilization*/
    for (int i = 0; i < parameters.n_thermalization_steps; i++) {
        metropolis_update(&lattice);
        /*.DAT WRITING:*/
        // fwrite(lattice.s, sizeof(char), L*L, gif_file);
    }
    // fclose(gif_file);

    /*Measure everything after initial stabilization*/
    measure_observables(&parameters, &lattice, energy, magnetization);

    for (int i = 0; i < parameters.n_measurements; i++) {
        abs_magnetization[i] = fabs(magnetization[i]);
    }

    if (parameters.configuration_flag == 0) {
        print_analysis_histogram(energy, magnetization, parameters.n_measurements,parameters.L);
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


/*
                                        DIFFERENT BETAS MAIN:
                                        DIFFERENT BETAS MAIN:
                                        DIFFERENT BETAS MAIN:
                                        DIFFERENT BETAS MAIN:
                                        DIFFERENT BETAS MAIN:
                                        DIFFERENT BETAS MAIN:
                                        DIFFERENT BETAS MAIN:
                                        DIFFERENT BETAS MAIN:
                                        DIFFERENT BETAS MAIN:
                                        DIFFERENT BETAS MAIN:
                                        DIFFERENT BETAS MAIN:
                                        DIFFERENT BETAS MAIN:
                                        DIFFERENT BETAS MAIN:
                                        DIFFERENT BETAS MAIN:
                                        DIFFERENT BETAS MAIN:
*/
int main() {
    int* Llist;
    int n_sims, initial_l, final_l;
    initial_l = 16;
    final_l = 128;
    n_sims = final_l-initial_l+1;
    // n_sims = 4;

    Llist = (int* )malloc(n_sims*sizeof(int));
    for(int i=0; i < n_sims; i++) {
        Llist[i] = initial_l+i;
        // Llist[i] = initial_l*(pow(2,i));
    }
    
    double cnt;
    double percentage;

    for (int sim = 0; sim < n_sims; sim++) {
        cnt = 0;
        int L = Llist[sim];
        int n_measurements = 10; /*Times the simulation will run to then take the average and desvest -- if doing hysteresis, this has to be set to 1*/
        int n_metropolis_steps = 10; /*Monte Carlo iterations (whole lattice spin flips) between each measurement -- if we make this higher, we let probability do its thing*/
        int n_thermalization_steps = 25*L; /*Initial stabilization steps -- if we make this higher, hysteresis will be negligible*/
        

        /*Beggining of code for beta distribution calculation -- taken from "beta_distribution_check.c" dear diory b i c h*/
        int n_betas = 100;
        double *betas;
        betas = (double*)calloc(n_betas, sizeof(double));

        double initial_beta, final_beta, delta_beta, critical_beta;
        initial_beta = 0.001; /*These values are very important, don't touch them*/
        final_beta = 0.999; /*These values are very important, don't touch them*/
        delta_beta = (final_beta-initial_beta)/n_betas;
        critical_beta = 0.44068679;

        //Logistic function parameters
        double A, beta_density_interval;
        beta_density_interval = 0.015; /* Centered in beta_critical, this will be the length of the interval that contains 76% of the betas*/
        A = 4/beta_density_interval; /* 4/A is the interval with predominant beta density (~76,16%) */

        //Betas array
        betas[0] = 0.0;
        for (int i=1; i<n_betas-1; i++) {
            betas[i] = initial_beta+i*delta_beta;   //Step 1: we generate an evenly-spaced beta distribution
            betas[i] = -1/A * log(1/betas[i]-1) + critical_beta;  //Step 2: we now apply the inverse logistic function
        }
        betas[n_betas-1] = 1.0;
        /*End of code for beta distribution calculation -- taken from "beta_distribution_check.c"*/
        // double betas[] = {0, 0.2, 0.3, 0.399, 0.440, 0.441, 0.442, 0.443, 0.6, 0.7, 1}; //0.44068679 is critical beta
        // int n_betas = 1;
        // double betas[] = {0.44068679};

        char filename[256];
        sprintf(filename, "results/simulations_L%03d.txt", L);
        FILE *f = fopen(filename, "w");

        if (f == NULL) {
            printf("no se ha podido abrir el fichero");
            return 1;
        }

        fprintf(f, "%8s   %12s   %13s   %18s   %14s\n", 
                "beta", "energy", "spec-heat", "abs-magnet", "suscept");


        // We'll parallelize optimizing CPU performance now:
        int n_threads;
        n_threads = omp_get_num_procs()*MULTIPLIER;               // Here's where the MULTIPLIER comes in
        if (n_threads == omp_get_num_procs()) {
            n_threads = n_threads-1;
        }
        n_threads = 15; //Optimized for ilaky's computer -- if you're not ilaky and this is not commented, comment it
        // n_threads = 1;
        omp_set_num_threads(n_threads);
        #pragma omp parallel for
        for (int i = 0; i < n_betas; i++) {
            /*It is VERY IMPORTANT that variables are PRIVATE for each thread; I think it's obvious why -- ask me if you don't get it*/
            //Following that, beta has to be local too!
            // double local_beta = 1.0 - (i + 1) * 0.01; 
            double local_beta = betas[i];
            
            double m_energy, s_heat, m_mag, susc;

            //Here's the heavy load
            run_simulation(local_beta, L, n_measurements, n_metropolis_steps, n_thermalization_steps, 
                           &m_energy, &s_heat, &m_mag, &susc);
            
            /*It is CRITICAL that not two threads write in the file at the same time -- for they'll overwrite each other if they do; only one thread can run this simultaneously*/
            #pragma omp critical
            {
                fprintf(f, "%8.4f   %12.6f   %13.6f   %18.6f   %14.6f\n", 
                        local_beta, m_energy, s_heat, m_mag, susc);
                cnt++;
                percentage = cnt/n_betas*100;
                printf("Progress: %.2f%%\tL=%d Beta=%.5f calculada por hilo %d\n", percentage, L, local_beta, omp_get_thread_num()); //Optional setting: print progress done by each thread
            }
        }

        fclose(f);
    }
    free(Llist);
    return 0;
}
/*
                                        HYSTERESIS MAIN:
                                        HYSTERESIS MAIN:
                                        HYSTERESIS MAIN:
                                        HYSTERESIS MAIN:
                                        HYSTERESIS MAIN:
                                        HYSTERESIS MAIN:
                                        HYSTERESIS MAIN:
                                        HYSTERESIS MAIN:
                                        HYSTERESIS MAIN:
                                        HYSTERESIS MAIN:
                                        HYSTERESIS MAIN:
                                        HYSTERESIS MAIN:
                                        HYSTERESIS MAIN:
                                        HYSTERESIS MAIN:
                                        HYSTERESIS MAIN:
*/
// int main() {
//     int n_threads;
//     n_threads = omp_get_num_procs()*MULTIPLIER;               // Here's where the MULTIPLIER comes in
//     n_threads = 15; //Optimized for ilaky's computer -- if you're not ilaky and this is not commented, comment it
//     omp_set_num_threads(n_threads);

//     int L = 128; //32 and 64 are left

//     #pragma omp parallel for
//     for (int i = 0; i < n_threads; i++) {
//         /*It is VERY IMPORTANT that variables are PRIVATE for each thread; I think it's obvious why -- ask me if you don't get it*/
//         Parameters P_local = {
//             .L = L,
//             .seed = 42 + i, /*THIS IS VITAL; WE NEED A DIFFERENT SEED FOR EACH*/
//             .n_measurements = 10,
//             .n_metropolis_steps = 10,
//             .n_thermalization_steps = L,
//             .beta_initial = 0.0,
//             .beta_final = 1.0,
//             .beta_step = 0.01
//         };

//         /*Local memory (every single thread need a different energy & magnetization vectors with different RAM directions)*/
//         Lattice L_local;

//         double *energy_local = malloc(sizeof(double) * P_local.n_measurements);
//         double *magnetization_local = malloc(sizeof(double) * P_local.n_measurements);
//         double *abs_magnetization_local = malloc(sizeof(double) * P_local.n_measurements);

//         ini_ran(P_local.seed); 
//         initialize_lattice_and_measurements(P_local, &L_local, &energy_local, &magnetization_local, &abs_magnetization_local);
//         build_direction_vectors(L_local.xp, L_local.yp, L_local.xm, L_local.ym, L_local.L);

//         /*Unique filename*/
//         char thread_filename[256];
//         sprintf(thread_filename, "results/hysteresis_L%d_run_%d.txt", L, i);

//         HisteresisMod(&L_local, P_local, energy_local, magnetization_local, thread_filename);

//         /*We now need to free our RAM*/
//         free_lattice_and_measurements(&L_local, energy_local, magnetization_local, abs_magnetization_local);
//     }
//     printf("All hysteresis are over\n");
//     return 0;
// }