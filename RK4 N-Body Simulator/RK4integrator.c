#include "head.h"

void CalculateAccelerations(int n, double* masses, double* positions, double* accelerations){
    //Initialize to zero
    for (int i=0; i<n; i++){
        accelerations[DIM*i + 0] = 0.0;
        accelerations[DIM*i + 1] = 0.0;
        accelerations[DIM*i + 2] = 0.0;
    }

    //Pairwise gravitational acceleration
    for (int i=0; i<n; i++){
        for (int j=0; j<n; j++){
            if (i != j){ //Skip self-gravity calculation
                double r[DIM];
                r[0] = positions[DIM*i + 0] - positions[DIM*j + 0];
                r[1] = positions[DIM*i + 1] - positions[DIM*j + 1];
                r[2] = positions[DIM*i + 2] - positions[DIM*j + 2];
                
                double normsqd = sqnorm(r, DIM);
                
                if (normsqd > EPSILON){
                    double factor = -G * masses[j] / (normsqd*sqrt(normsqd));
                    accelerations[DIM*i + 0] += factor*r[0];
                    accelerations[DIM*i + 1] += factor*r[1];
                    accelerations[DIM*i + 2] += factor*r[2];
                }
            }
        }
    }
}

void GravityEngine(double dt, Body* bodies, int n){
    double positions[DIM*N_BODIES];
    double velocities[DIM*N_BODIES];
    double masses[N_BODIES];
    
    //Set to current state
    for (int i= 0; i<n; i++){
        positions[DIM*i + 0] = bodies[i].pos[0];
        positions[DIM*i + 1] = bodies[i].pos[1];
        positions[DIM*i + 2] = bodies[i].pos[2];
        velocities[DIM*i + 0] = bodies[i].vel[0];
        velocities[DIM*i + 1] = bodies[i].vel[1];
        velocities[DIM*i + 2] = bodies[i].vel[2];
        masses[i] = bodies[i].mass;
    }
    
    //Arrays for RK4 "ghost" states
    double ghost_pos[DIM*N_BODIES];
    double k1_v[DIM*N_BODIES], k1_r[DIM*N_BODIES];
    double k2_v[DIM*N_BODIES], k2_r[DIM*N_BODIES];
    double k3_v[DIM*N_BODIES], k3_r[DIM*N_BODIES];
    double k4_v[DIM*N_BODIES], k4_r[DIM*N_BODIES];

    //k1
    for (int i= 0; i<n; i++){
        k1_r[DIM*i + 0] = velocities[DIM*i + 0];
        k1_r[DIM*i + 1] = velocities[DIM*i + 1];
        k1_r[DIM*i + 2] = velocities[DIM*i + 2];
    }
    CalculateAccelerations(n, masses, positions, k1_v);

    //k2
    for (int i =0; i<n; i++){
        ghost_pos[DIM*i + 0] = positions[DIM*i + 0] + (k1_r[DIM*i + 0] * dt / 2.0);
        ghost_pos[DIM*i + 1] = positions[DIM*i + 1] + (k1_r[DIM*i + 1] * dt / 2.0);
        ghost_pos[DIM*i + 2] = positions[DIM*i + 2] + (k1_r[DIM*i + 2] * dt / 2.0);
        k2_r[DIM*i + 0] = velocities[DIM*i + 0] + (k1_v[DIM*i + 0] * dt / 2.0);
        k2_r[DIM*i + 1] = velocities[DIM*i + 1] + (k1_v[DIM*i + 1] * dt / 2.0);
        k2_r[DIM*i + 2] = velocities[DIM*i + 2] + (k1_v[DIM*i + 2] * dt / 2.0);
    }
    CalculateAccelerations(n, masses, ghost_pos, k2_v);

    //k3
    for (int i= 0; i<n; i++){
        ghost_pos[DIM*i + 0] = positions[DIM*i + 0] + (k2_r[DIM*i + 0] * dt / 2.0);
        ghost_pos[DIM*i + 1] = positions[DIM*i + 1] + (k2_r[DIM*i + 1] * dt / 2.0);
        ghost_pos[DIM*i + 2] = positions[DIM*i + 2] + (k2_r[DIM*i + 2] * dt / 2.0);
        k3_r[DIM*i + 0] = velocities[DIM*i + 0] + (k2_v[DIM*i + 0] * dt / 2.0);
        k3_r[DIM*i + 1] = velocities[DIM*i + 1] + (k2_v[DIM*i + 1] * dt / 2.0);
        k3_r[DIM*i + 2] = velocities[DIM*i + 2] + (k2_v[DIM*i + 2] * dt / 2.0);
    }
    CalculateAccelerations(n, masses, ghost_pos, k3_v);

    //k4
    for (int i= 0; i<n; i++){
        ghost_pos[DIM*i + 0] = positions[DIM*i + 0] + (k3_r[DIM*i + 0] * dt);
        ghost_pos[DIM*i + 1] = positions[DIM*i + 1] + (k3_r[DIM*i + 1] * dt);
        ghost_pos[DIM*i + 2] = positions[DIM*i + 2] + (k3_r[DIM*i + 2] * dt);
        k4_r[DIM*i + 0] = velocities[DIM*i + 0] + (k3_v[DIM*i + 0] * dt);
        k4_r[DIM*i + 1] = velocities[DIM*i + 1] + (k3_v[DIM*i + 1] * dt);
        k4_r[DIM*i + 2] = velocities[DIM*i + 2] + (k3_v[DIM*i + 2] * dt);
    }
    CalculateAccelerations(n, masses, ghost_pos, k4_v);

    //Update positions & velocities
    for (int i= 0; i<n; i++){
        bodies[i].pos[0] = positions[DIM*i + 0] + (dt/6.0)*
        (k1_r[DIM*i + 0]+
        2.0*k2_r[DIM*i + 0]+
        2.0*k3_r[DIM*i + 0]+
        k4_r[DIM*i + 0]);

        bodies[i].pos[1] = positions[DIM*i + 1] + (dt/6.0)*
        (k1_r[DIM*i + 1]+
        2.0*k2_r[DIM*i + 1]+
        2.0*k3_r[DIM*i + 1]+
        k4_r[DIM*i + 1]);

        bodies[i].pos[2] = positions[DIM*i + 2] + (dt/6.0)*
        (k1_r[DIM*i + 2]+
        2.0*k2_r[DIM*i + 2]+
        2.0*k3_r[DIM*i + 2]+
        k4_r[DIM*i + 2]);

        bodies[i].vel[0] = velocities[DIM*i + 0] + (dt/6.0)*
        (k1_v[DIM*i + 0]+
        2.0*k2_v[DIM*i + 0]+
        2.0*k3_v[DIM*i + 0]+
        k4_v[DIM*i + 0]);

        bodies[i].vel[1] = velocities[DIM*i + 1] + (dt/6.0)*
        (k1_v[DIM*i + 1]+
        2.0*k2_v[DIM*i + 1]+
        2.0*k3_v[DIM*i + 1]+
        k4_v[DIM*i + 1]);

        bodies[i].vel[2] = velocities[DIM*i + 2] + (dt/6.0)*
        (k1_v[DIM*i + 2]+
        2.0*k2_v[DIM*i + 2]+
        2.0*k3_v[DIM*i + 2]+
        k4_v[DIM*i + 2]);
    }
}