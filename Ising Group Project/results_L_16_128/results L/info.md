These were ran with:

    int main() {
    int* Llist;
    int n_sims, initial_l, final_l;
    initial_l = 16;
    final_l = 512;
    n_sims = final_l-initial_l+1;
    n_sims = 4;
    Llist = (int* )malloc(n_sims*sizeof(int));
    for(int i=0; i < n_sims; i++) {
        // Llist[i] = initial_l+i;
        Llist[i] = initial_l*(pow(2,i));
    }
    
    double cnt;
    double percentage;

    for (int sim = 0; sim < n_sims; sim++) {
        cnt = 0;
        int L = Llist[sim];
        int n_measurements = 100000; /*Times the simulation will run to then take the average and desvest -- if doing hysteresis, this has to be set to 1*/
        int n_metropolis_steps = 100; /*Monte Carlo iterations (whole lattice spin flips) between each measurement -- if we make this higher, we let probability do its thing*/
        int n_thermalization_steps = 25*L; /*Initial stabilization steps -- if we make this higher, hysteresis will be negligible*/

        double betas[] = {0.44068679};
