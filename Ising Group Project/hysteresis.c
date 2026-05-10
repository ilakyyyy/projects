#include "ising.h"
void initialize_probability_array(Lattice* lattice, double beta);

void Histeresis (Lattice *L, Parameters P,double *energy,double *magnetization){
    //Inicializamos lo q vamos a necesitar
    int N_pasos,sentido,N_betas,N_Met,i; //creo delta beta para poder modificarlo sin modificar p
    double beta,med_mag,var_mag,delta_beta;
    delta_beta=P.beta_step;

     FILE *f=fopen ("results/Histeresis.txt","w");
    //Generamos la primera conf
    generate_configuration(&P, (*L).s);
    N_pasos=(fabs(P.beta_final-P.beta_initial)/delta_beta);
    beta=P.beta_initial;//de nuevo, para poder ir cambiando el beta
    for(sentido=0;sentido<2;sentido++){ //adelante y para atras
        for(N_betas=0;N_betas<N_pasos;N_betas++){
            initialize_probability_array(L, beta);
            for(N_Met=0;N_Met<P.n_thermalization_steps;N_Met++) //Thermalización antes de comenzar las medidas
                metropolis_update(L);
            //Inicializas energy y magnetization como 0 para esta iteración
            for (i=0;i<P.n_measurements;i++){
                energy[i]=0;
                magnetization[i]=0;
            }
            //La siguiente parte del codigo se resuma en emplear la función measure
            measure_observables(&P, L, energy, magnetization);
            for (int i = 0; i < P.n_measurements; i++) {
                magnetization[i] = fabs(magnetization[i]);}

            med_var(magnetization, P.n_measurements, &med_mag, &var_mag);
            fprintf(f,"%g \t %g  \n", beta, med_mag);
            printf("%g \t %g  \n", beta, med_mag);
            beta+=delta_beta;}
        delta_beta=-delta_beta;
    }
    fclose(f);
   }


    