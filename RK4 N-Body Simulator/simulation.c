#include "head.h"
#include <string.h>

int main() {
    //=======================================
    //Initial parameters
    //=======================================
    int skip = 100;
    int fps = 80;
    
    // double min_yaw_angle = 1.74500;
    // double max_yaw_angle = 1.74500;
    // double min_pitch_angle = -2.10500;
    // double max_pitch_angle = -2.10500;

    double min_yaw_angle = -8.39500;
    double max_yaw_angle = -8.39500;
    double min_pitch_angle = -0.82000;
    double max_pitch_angle = -0.82000;
    
    double total_t = 2000.0;
    double factor = 0.05;
    double precision_factor = 50.0;
    double max_dt = 50;
    double min_dt = 0.01;
    int pitch_sims = 1;
    int yaw_sims = 1;
    int total_sims = yaw_sims*pitch_sims;

    // int skip = 100;
    // int fps = 80;
    
    // double min_yaw_angle = -180.0;//Yaw (phi)
    // double max_yaw_angle = 180.0;
    // double min_pitch_angle = -180.0;//Pitch (theta)
    // double max_pitch_angle = 180.0;
    
    // double total_t = 1200.0;//days
    // double factor = 0.005;//lower values => more precision
    // double precision_factor = 80.0;//will divite dt by it when closer than 1e9km
    // double max_dt = 10;
    // double min_dt = 0.001;
    // int pitch_sims = 15;//every 12º
    // int yaw_sims = 30;//every 12º
    // int total_sims = yaw_sims*pitch_sims;



    //malloc
    double* yaw_angles = malloc(yaw_sims*sizeof(double));
    CalculateSweep(min_yaw_angle, max_yaw_angle, yaw_angles, yaw_sims);
    
    double* pitch_angles = malloc(pitch_sims*sizeof(double));
    CalculateSweep(min_pitch_angle, max_pitch_angle, pitch_angles, pitch_sims);

    //=======================================
    // Main Execution Loop
    //=======================================
    time_t start_time = time(NULL);
    int completed = 0;
    int valid_completed = 0;

    //read .txt from python
    char base_folder[MAX_CHAR] = "simulation_results";
    FILE* config_file = fopen("batch_path.txt", "r");
    if (config_file != NULL) {
        fscanf(config_file, "%s", base_folder);
        fclose(config_file);
    }

    //load JPL ephemeris data
    double ephem[6][6];
    FILE* eph_file = fopen("ephemeris.txt", "r");
    if (eph_file == NULL) {
        printf("ephemeris.txt not found!\n");
        return 1;
    }
    for (int i=0; i<6; i++) {
        fscanf(eph_file, "%lf %lf %lf %lf %lf %lf", &ephem[i][0], &ephem[i][1], &ephem[i][2], &ephem[i][3], &ephem[i][4], &ephem[i][5]);
    }
    fclose(eph_file);

    //JPL Data
    double ship_vx_jpl = ephem[5][3];
    double ship_vy_jpl = ephem[5][4];
    double ship_vz_jpl = ephem[5][5];
    double exact_vel = sqrt(ship_vx_jpl*ship_vx_jpl + ship_vy_jpl*ship_vy_jpl + ship_vz_jpl*ship_vz_jpl);
    
    double* ship_velocities;
    ship_velocities = malloc(total_sims*sizeof(double));
    CalculateSweep(exact_vel, exact_vel, ship_velocities, total_sims);

    //schedule(dynamic) ensures threads take on a new sim when they finish one
    #pragma omp parallel for schedule(dynamic)
    for (int i=0; i<total_sims; i++) {
        char current_output_folder[MAX_CHAR * 2];
        snprintf(current_output_folder, sizeof(current_output_folder), "%s/sim_%d", base_folder, i+1);

        //calculate which is this thread simulating
        int yaw_idx = i % yaw_sims;
        int pitch_idx = i / yaw_sims;
        double current_yaw = yaw_angles[yaw_idx];
        double current_pitch = pitch_angles[pitch_idx];

        int status = RunSimulation(current_output_folder, current_yaw, current_pitch, ship_velocities[i], total_t, factor, i, skip, fps, precision_factor, max_dt, min_dt, ephem);

        #pragma omp critical 
        {
            completed++;
            if (status == 1) {
                valid_completed++;
            }
            
            double completion = (completed*100.0)/total_sims;
            time_t current_time = time(NULL);
            double elapsed = difftime(current_time, start_time);
            
            //Exclude collisions from counting towards ETA
            double eta = 0.0;
            if (valid_completed > 0) {
                double time_per_valid = elapsed/valid_completed;
                eta = time_per_valid*(total_sims-completed);
            }
            
            printf("[%3.0f%%] ETA: %02dm %02ds | Completed Sim ID %d / %d\n", completion, ((int)eta)/60, ((int)eta)%60, i+1, total_sims);
        }
    }
    time_t end_time = time(NULL);
    double total_elapsed = difftime(end_time, start_time);
    printf("[100%%] All simulations completed successfully in %02dm %02ds.\n", ((int)total_elapsed)/60, ((int)total_elapsed)%60);

    free(yaw_angles);
    free(pitch_angles);
    free(ship_velocities);
    return(0);
}