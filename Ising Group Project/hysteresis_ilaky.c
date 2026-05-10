#include "ising.h"
void initialize_probability_array(Lattice* lattice, double beta);

void write_medvars(FILE *f, double beta, double med_enrg, double med_mag, double med_enrg2, double med_mag2, double var_energy, double var_mag) {
    fprintf(f,"%g\t%g\t%g\t%g\t%g\n", beta, med_enrg, med_mag, med_enrg2, med_mag2); //%g is to print numbers in the shortest format -- 1,23 or 3E29
}

void HisteresisMod(Lattice *L, Parameters P, double *energy, double *magnetization, char *filename){ //energy y magnetization deben ser arrays de P.N_measurements componentes
    //Inicializamos lo q vamos a necesitar
    int N_pasos, sentido, N_betas, N_Met, i; //creo delta beta para poder modificarlo sin modificar p
    double beta, delta_beta;
    double med_mag, var_mag, med_energy, var_energy; // media y varianza para energía y magnetización
    double med_mag2, var_mag2, med_energy2, var_energy2; // media y varianza de las anteriores al cuadrado
    double *energy2, *magnetization2; // vectores por componentes del cuadrado de energy y magnetization
    energy2 = malloc(sizeof(double)*P.n_measurements);
    magnetization2 = malloc(sizeof(double)*P.n_measurements); 
    delta_beta = P.beta_step;
    FILE *f=fopen (filename,"w");
    if (f == NULL){ 
        printf("Error al abrir %s\n", filename); 
        return;
    }

    //We generate starting config
    generate_configuration(&P, (*L).s);
    N_pasos = (fabs(P.beta_final-P.beta_initial)/delta_beta);
    beta = P.beta_initial; //We set starting beta to the initial chosen when calling the function
    for(sentido = 0;  sentido < 2;    sentido++){ //Go forward, and then backwards
        for(N_betas = 0;    N_betas < N_pasos;  N_betas++){

            initialize_probability_array(L, beta);
            for(N_Met=0;    N_Met < P.n_thermalization_steps;  N_Met++) {//Initial thermalization before measuring (to go to the center of the gaussian)
                metropolis_update(L);
            }

            for (i=0;   i < P.n_measurements;   i++){ //We set to 0 every single index in energy and magnetization
                energy[i]=0;
                magnetization[i]=0;
                energy2[i]=0;
                magnetization2[i]=0;
            }

            measure_observables(&P, L, energy, magnetization);
            // N_m := N_measurements        N_M := N_montecarlo_iterations
            for (int N_m = 0;   N_m < P.n_measurements; N_m++) { //We want N_m measurements -- already in metropolis!!!
                magnetization[N_m] = fabs(magnetization[N_m]);              // valor absoluto para magnetización
                magnetization2[N_m] = magnetization[N_m]*magnetization[N_m]; 
                energy2[N_m] = energy[N_m]*energy[N_m];                       // valores para energía y magnet ^2
            }

            // !!!!!!!!!!!!!! --- ATENCIÓN --- !!!!!!!!!!!!!!!!!!!!

            // EPSILON ES SIGMA(VARIANZA) / RAIZ(N)     CON N = N_MEASUREMENTS

            // !!!!!!!!!!!!!! --- ATENCIÓN --- !!!!!!!!!!!!!!!!!!!!


            med_var(magnetization, P.n_measurements, &med_mag, &var_mag);       // calc magnetization avg and var
            med_var(energy, P.n_measurements, &med_energy, &var_energy);        // calc energy avg and var
            med_var(magnetization2, P.n_measurements, &med_mag2, &var_mag2);    // calc magnetization^2 avg and var
            med_var(energy2, P.n_measurements, &med_energy2, &var_energy2);     // calc energy^2 avg and var
            
            write_medvars(f, beta, med_energy, med_mag, med_energy2, med_mag2, var_energy, var_mag);
            // printf("%g \t %g  \n", beta, med_mag); //This serves to check if evth's working as it should (prints on screen) -- uncomment if u wanna check
            beta += delta_beta;
        }
    delta_beta = -delta_beta; //invert beta increments and do everything again
   }
//    free(energy2); free(magnetization2); // no sé, tío
   fclose(f);
}

    