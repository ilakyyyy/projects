#include "head.h"

//Helper functions
double norm(double* vec, int n) {
    double sum = 0.0;
    for(int i=0; i<n; i++) {
        sum += vec[i]*vec[i];
    }
    return(sqrt(sum));
}
double sqnorm(double* vec, int n) {
    double sum = 0.0;
    for(int i=0; i<n; i++) {
        sum += vec[i]*vec[i];
    }
    return(sum);
}
void vecminus(const double* vec2, const double* vec1, double* result, int n) {
    for(int i=0; i<n; i++) result[i] = vec2[i]-vec1[i];
}

double CalculateHamiltonian(Body* bodies, int n) {
    double T, V;
    T = V = 0.0;
    double r[DIM];
    for(int i=0; i<n; i++) {
        T += 0.5 * bodies[i].mass*sqnorm(bodies[i].vel, DIM);
        for(int j=i+1; j<n; j++) {//Avoid counting pairs twice
            vecminus(bodies[i].pos, bodies[j].pos, r, DIM);
            V += -G * bodies[i].mass*bodies[j].mass/norm(r, DIM);
        }
    }
    return(T+V);
}

double ShipHamiltonian(Body* bodies, int n) {
    double T, V, r[DIM];
    T = V = 0.0;
    T = 0.5 * bodies[SHIP_INDEX].mass*sqnorm(bodies[SHIP_INDEX].vel, DIM);
    for(int i=0; i<n; i++) {
        if(i!=SHIP_INDEX) {
            vecminus(bodies[i].pos, bodies[SHIP_INDEX].pos, r, DIM);
            V+= -G * bodies[i].mass*bodies[SHIP_INDEX].mass / norm(r, DIM);
        }
    }
    return(T+V);
}

double MinimumDistance(Body* bodies, int index) {
    double r[DIM];
    vecminus(bodies[SHIP_INDEX].pos, bodies[index].pos, r, DIM);
    return(norm(r, DIM));
}

void CalculateSweep(double min, double max, double* sweep, int n) {
    double range = max - min;
    if (n==1) {
        *sweep = min; // Matching np.linspace behavior for n=1
        return;
    }
    double step = range/(n-1);
    for(int i=0; i<n; i++) *(sweep+i) = min+step*i;
}

int RunSimulation(char* output_folder, double launch_angle, double pitch_angle, double initial_ship_v, double total_time, double factor, 
int sim_id, int skip, int fps, double precision_factor, double max_dt, double min_dt, double ephem[6][6]) {
    double e_x = ephem[2][0];
    double e_y = ephem[2][1];
    double e_z = ephem[2][2];

    double ship_r_x = e_x*(1.0 - 1.0/999.0);
    double ship_r_y = e_y*(1.0 - 1.0/999.0);
    double ship_r_z = e_z*(1.0 - 1.0/999.0);
    
    //Base velocity vector aligned with that of Earth
    double e_v[3];
    e_v[0] = ephem[2][3];
    e_v[1] = ephem[2][4];
    e_v[2] = ephem[2][5];
    double e_v_norm = norm(e_v, DIM);
    double base_vx = (e_v[0]/e_v_norm)*initial_ship_v;
    double base_vy = (e_v[1]/e_v_norm)*initial_ship_v;
    double base_vz = (e_v[2]/e_v_norm)*initial_ship_v;
    
    //Convert base velocity to spherical coords
    double xy_norm = sqrt(base_vx*base_vx + base_vy*base_vy);
    double phi = atan2(base_vy, base_vx);
    double theta = atan2(xy_norm, base_vz);
    
    //Yaw and Pitch sweeps
    double new_phi = phi + launch_angle*(PI/180.0);
    double new_theta = theta + pitch_angle*(PI/180.0);
    
    double ship_v_x = initial_ship_v*sin(new_theta)*cos(new_phi);
    double ship_v_y = initial_ship_v*sin(new_theta)*sin(new_phi);
    double ship_v_z = initial_ship_v*cos(new_theta);

    //Ephemeris data initialization
    Body bodies[N_BODIES] = {
        [SHIP_INDEX]  = {"Ship",  SHIP_MASS,  {ship_r_x, ship_r_y, ship_r_z}, {ship_v_x, ship_v_y, ship_v_z}},
        [SUN_INDEX]   = {"Sun",   SUN_MASS,   {ephem[0][0], ephem[0][1], ephem[0][2]}, {ephem[0][3], ephem[0][4], ephem[0][5]}},
        [VENUS_INDEX] = {"Venus", VENUS_MASS, {ephem[1][0], ephem[1][1], ephem[1][2]}, {ephem[1][3], ephem[1][4], ephem[1][5]}},
        [EARTH_INDEX] = {"Earth", EARTH_MASS, {ephem[2][0], ephem[2][1], ephem[2][2]}, {ephem[2][3], ephem[2][4], ephem[2][5]}},
        [MOON_INDEX]  = {"Moon",  MOON_MASS,  {ephem[3][0], ephem[3][1], ephem[3][2]}, {ephem[3][3], ephem[3][4], ephem[3][5]}},
        [JUPITER_INDEX]={"Jupiter",JUPITER_MASS,{ephem[4][0], ephem[4][1], ephem[4][2]}, {ephem[4][3], ephem[4][4], ephem[4][5]}}
    };
    
    double sim_time = 0.0, dt;
    double max_time = total_time * 24.0 * 3600.0;
    double min_distance = 1e30; //Initialize to a very large number
    
    //Prepare output file paths
    char bin_filename[256];
    char txt_filename[256];
    snprintf(bin_filename, sizeof(bin_filename), "%s/simulation_data_%d.bin", output_folder, sim_id+1);
    snprintf(txt_filename, sizeof(txt_filename), "%s/simulation_info_%d.txt", output_folder, sim_id+1);
    
    FILE* bin_file = fopen(bin_filename, "wb");
    if (bin_file == NULL) {
        printf("Couldn't create output file %s; kake sure the output folder exists.\n", bin_filename);
        return 0;
    }
    
    long int steps = 0;
    double initial_energy = CalculateHamiltonian(bodies, N_BODIES);
    double final_energy = initial_energy;
    
    double initial_ship_energy = ShipHamiltonian(bodies, N_BODIES);
    double final_ship_energy = initial_ship_energy;

    //=======================================
    //Main Simulation Loop
    //=======================================
    while (sim_time < max_time) {
        double current_energy = CalculateHamiltonian(bodies, N_BODIES);
        double current_ship_energy = ShipHamiltonian(bodies, N_BODIES);
        final_energy = current_energy;
        final_ship_energy = current_ship_energy;

        double venus_dist = MinimumDistance(bodies, VENUS_INDEX);
        if (venus_dist < min_distance) {
            min_distance = venus_dist;
        }

        if (min_distance <= COLLISION_DISTANCE) {
            fclose(bin_file);
            remove(bin_filename);
            
            char new_folder[MAX_CHAR * 2 + 32];
            snprintf(new_folder, sizeof(new_folder), "%s_COLLISION", output_folder);
            rename(output_folder, new_folder);
            return 0;
        }

        //15 doubles per step:
        //time_days, total_energy, ship_energy, ship_x, ship_y, ship_z...
        if (steps % skip == 0) {
            double row_data[3+ N_BODIES*DIM];
            row_data[0] = sim_time/(24.0 * 3600.0); //conversion of time (s) -> (d)
            row_data[1] = current_energy;
            row_data[2] = current_ship_energy;
            for (int i = 0; i < N_BODIES; i++) {
                row_data[3 + DIM*i + 0] = bodies[i].pos[0];//x_pos
                row_data[3 + DIM*i + 1] = bodies[i].pos[1];//y_pos
                row_data[3 + DIM*i + 2] = bodies[i].pos[2];//z_pos
            }
            fwrite(row_data, sizeof(double), 3+N_BODIES*DIM, bin_file);
        }
        steps++;

        //Adaptive step size based on current distance to Venus
        dt = factor * pow(venus_dist, 1.0/3.0);
        if (dt > max_dt) dt = max_dt;
        if (dt < min_dt) dt = min_dt;
        if (venus_dist < 1e9) dt /= precision_factor;

        GravityEngine(dt, bodies, N_BODIES);
        sim_time += dt;
    }
    fclose(bin_file);
    
    //=======================================
    //Write Text Info Report
    //=======================================
    FILE* txt_file = fopen(txt_filename, "w");
    if (txt_file == NULL) {
        printf("Couldn't create output file %s; kake sure the output folder exists.\n", txt_filename);
        return 0;
    }
    double final_ship_v = norm(bodies[SHIP_INDEX].vel, DIM);
    double delta_v = final_ship_v - initial_ship_v;
    double delta_ship_energy = final_ship_energy - initial_ship_energy;
    double final_energy_error = (final_energy - initial_energy)/fabs(initial_energy);
    
    fprintf(txt_file, "SIMULATION REPORT - ID %d\n=======================================\n\n", sim_id + 1);
    fprintf(txt_file, "Initial conditions:\n  Launch Angle:      %f degrees\n  Pitch Angle:       %f degrees\n  Ship Velocity:     %f m/s\n  Bodies:            ['Ship', 'Sun', 'Venus', 'Earth', 'Moon', 'Jupiter']\n\n", launch_angle, pitch_angle, initial_ship_v);
    fprintf(txt_file, "Time settings:\n  Total Steps:       %ld\n\nPerformance:\n  Factor:            %f\n  Precision Factor:  %f   Max dt: %f\n    Min dt: %f\n  Relative Energy Error: %.5e\n  Final Ship Velocity:   %.0f m/s\n  Delta Ship Velocity:   %.0f m/s\n  Delta Ship Energy:     %.5e J\n  Minimum Ship Distance: %.0f m\n", steps, factor, precision_factor, max_dt, min_dt, final_energy_error, final_ship_v, delta_v, delta_ship_energy, min_distance);
    fclose(txt_file);

    return 1;
}