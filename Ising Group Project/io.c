#include "ising.h"

/**
 * Saves the given spin configuration 's' to a file specified by 'filename'.
 * The configuration is saved in a human-readable format, with each spin value
 * separated by a space and each row of the lattice on a new line.
 */
void save_configuration(const char* filename, char* s, int L) {
   int i,j;
    FILE *f=fopen( filename,"w");
    for (j=0;j<L;j++){
    for (i=0;i<L;i++){
        fprintf(f,"%d \t",s[i+j*L]);
    }
    fprintf(f,"\n");}
    fclose(f);}

void save_histogram(const char* filename, double* hist, int n_bins, double delta, double min) {
    FILE *f=fopen(filename, "w");
    if(f==NULL){
        printf("no se ha podido abrir el fichero");
        return; 
    }

    for(int i=0;i<n_bins;i++){
        fprintf(f,"%lf\t%lf\n", min+(i+0.5)*delta, hist[i]);
    }
    fclose(f);
}