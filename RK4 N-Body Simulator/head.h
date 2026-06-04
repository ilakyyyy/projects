#ifndef HEAD_H
#define HEAD_H

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <omp.h>

#define PI 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825
#define MAX_CHAR 128
#define EPSILON 0.00001 //Will be useful when comparing doubles, to avoid division by zero
#define DIM 3 //Careful if changing this; you'll need to redefine first loop in RunSimulation

//Initial constants, S.I.
#define G 6.67430e-11
#define N_BODIES 6

//CAREFUL IF CHANGING PERIHELION DATA!!!!

//Venus - perihelion data:
#define VENUS_RADIUS 6.052e6
#define COLLISION_DISTANCE 6.152e6 //Venus radius(6052km)+Atmosphere(let's make it ~100 km so as to not lose any energy to drag)
#define VENUS_MASS 4.8673058e24
#define VENUS_R0_P_X 1.0748e11
#define VENUS_R0_P_Y 0.0
#define VENUS_R0_P_Z 0.0
#define VENUS_V0_P_X 0.0
#define VENUS_V0_P_Y 35.26e3
#define VENUS_V0_P_Z 0.0
#define VENUS_INCLINATION 3.4 //Venus orbital tilt in degrees relative to the Ecliptic
#define VENUS_INDEX 2

//Earth - initial phase = 0:
#define EARTH_RADIUS 6.378e6
#define EARTH_MASS 5.9721684e24
#define EARTH_R0_X 1.4709e11
#define EARTH_R0_Y 0.0
#define EARTH_R0_Z 0.0
#define EARTH_V0_X 0.0
#define EARTH_V0_Y 30.29e3
#define EARTH_V0_Z 0.0
#define EARTH_INDEX 3

//Moon:
#define MOON_RADIUS 1.7375e6
#define MOON_MASS 7.3457892e22
#define MOON_R0_X (1.4709e11 + 3.63e8)
#define MOON_R0_Y 0.0
#define MOON_R0_Z 0.0
#define MOON_V0_X 0.0
#define MOON_V0_Y 31322.0
#define MOON_V0_Z 0.0
#define MOON_INDEX 4

//Jupiter:
#define JUPITER_RADIUS 7.1492e7
#define JUPITER_MASS 1.8985177e27
#define JUPITER_R0_X 7.4052e11
#define JUPITER_R0_Y 0.0
#define JUPITER_R0_Z 0.0
#define JUPITER_V0_X 0.0
#define JUPITER_V0_Y 13.72e3
#define JUPITER_V0_Z 0.0
#define JUPITER_INDEX 5

//Sun:
#define SUN_RADIUS 6.957e8
#define SUN_MASS 1.9884099e30
#define SUN_R0_X 0.0
#define SUN_R0_Y 0.0
#define SUN_R0_Z 0.0
#define SUN_V0_X 0.0
#define SUN_V0_Y 0.0
#define SUN_V0_Z 0.0
#define SUN_INDEX 1

//Ship:
#define SHIP_MASS 1.8e3
#define SHIP_R0_X (1.4709e11 * (1.0 - 1.0/999.0))
#define SHIP_R0_Y 0.0
#define SHIP_R0_Z 0.0
#define SHIP_INDEX 0 //Careful if changing this value; change in RumSim Bodies array initialization



//Data Structures
typedef struct {
    char name[MAX_CHAR];
    double mass;
    double pos[DIM];
    double vel[DIM];
} Body;

//functions.c
double norm(double* vec, int n);
double sqnorm(double* vec, int n);
void vecminus(const double* vec2, const double* vec1, double* result, int n);
double CalculateHamiltonian(Body* bodies, int n);
double ShipHamiltonian(Body* bodies, int n);
double MinimumDistance(Body* bodies, int index);
void CalculateSweep(double min, double max, double* sweep, int n);
int RunSimulation(char* output_folder, double launch_angle, double pitch_angle, double initial_ship_v, double total_time, double factor, 
int sim_id, int skip, int fps, double precision_factor, double max_dt, double min_dt, double ephem[6][6]);

//RK4integrator.c
void GravityEngine(double dt, Body* bodies, int n);


#endif