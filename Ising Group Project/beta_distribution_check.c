#include "ising.h"

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

void Histograma(double *data, double *Hist, int N_data, int N_intervalos, double *d, double *m, double *M) {
    minandmax(data, N_data, M, m);
    *d = (*M - *m) / N_intervalos;
    int k;
    for (int i = 0; i < N_data; i++) {
        k = (int)((*(data + i) - *m)/ *d);
        *(Hist + k) += 1;
    }
    for (int i = 0; i < N_intervalos; i++) {
        *(Hist + i) =  *(Hist + i) / N_data;
    }
}

int main() {
    //Histogram initialization
    int n_ints = 100;
    double* Histogram;
    Histogram = (double* )calloc(n_ints, sizeof(double));
    double delta;
    
    
    //Betas initialization
    int n_betas = 1000;
    double *betas;
    betas = (double*)calloc(n_betas, sizeof(double));

    double initial_beta, final_beta, delta_beta, critical_beta;
    initial_beta = 0.001; /*These values are very important, don't touch them*/
    final_beta = 0.999; /*These values are very important, don't touch them*/
    delta_beta = (final_beta-initial_beta)/n_betas;
    critical_beta = 0.44068679;

    //Logistic function parameters
    double A, beta_density_interval;
    beta_density_interval = 0.15; /* Centered in beta_critical, this will be the length of the interval that contains 76% of the betas*/
    A = 4/beta_density_interval; /* 4/A is the interval with predominant beta density (~76,16%) */

    //Betas array
    betas[0] = 0.0;
    for (int i=1; i<n_betas-1; i++) {
        betas[i] = initial_beta+i*delta_beta;   //Step 1: we generate an evenly-spaced beta distribution
        betas[i] = -1/A * log(1/betas[i]-1) + critical_beta;  //Step 2: we now apply the inverse logistic function
    }
    betas[n_betas-1] = 1.0;

    for(int i=0; i<n_betas; i++) {
        printf("%f\n", betas[i]);
    }

    Histograma(betas, Histogram, n_betas, n_ints, &delta, &initial_beta, &final_beta);
    for(int i=0; i<n_ints; i++) {
        printf("(%f,%f)\n", initial_beta+delta*i, Histogram[i]);
    }
}