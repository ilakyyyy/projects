import numpy as np
import math
# import integrator as int
import matplotlib.pyplot as plt
from tqdm import tqdm

class Body:
    def __init__(self, name, mass, pos, vel):
        self.name = name
        self.mass = mass
        # phase: float
        # radius: float
        # distance: float #(to Sun)
        self.pos = np.array(pos, dtype='float64')
        self.vel = np.array(vel, dtype='float64')


# Gravity engine:
# It should take everything before and change everything

# First step: Build the acceleration function (positions -> accelerations)
# Second step: 
# Positions: 1 - Body index; 2 - coord

def CalculateAcceleration(target_pos, att_pos, att_mass):
    r = target_pos - att_pos
    norm = np.linalg.norm(r)
    if norm == 0:
        print(f"Division by 0!")
        return(np.zeros(2))
    acc = -G*att_mass*r/(norm**3)
    return(acc)
def TotalAcceleration(index, positions, masses, n):
    total_acc = np.zeros(2)
    for j in range(n):
        if j != index:
            total_acc += CalculateAcceleration(positions[index], positions[j], masses[j])
    return(total_acc)

#For one instant of time
def GravityEngine(dt, bodies):
    # Sun = bodies[0]
    # Earth = bodies[1]
    # Venus = bodies[2]
    positions = np.zeros((len(bodies), 2))
    velocities = np.zeros((len(bodies), 2))
    masses = np.zeros(len(bodies))

    for i in range(len(bodies)):
        body = bodies[i]
        positions[i, :] = body.pos
        velocities[i, :] = body.vel
        masses[i] = body.mass
    
    # RK-4 Engine:
    n = len(bodies)
    # Calculate k1:
    k1_v = np.zeros((n, 2))
    k1_r = np.zeros((n, 2))
    for i in range(n):
        k1_r[i] = velocities[i]
        k1_v[i] = TotalAcceleration(i, positions, masses, n)
    # Calculate k2:
    k2_v = np.zeros((n, 2))
    k2_r = np.zeros((n, 2))
    k2_r_ghost = np.zeros((n, 2))
    #is this for necessary? no, we can vectorize (leave for later)
    for i in range(n):
        k2_r_ghost[i] = positions[i] + (k1_r[i] * dt/2)
    for i in range(n):
        k2_v[i] = TotalAcceleration(i, k2_r_ghost, masses, n)
        k2_r[i] = velocities[i] + (k1_v[i] * dt/2)
    #Calculate k3:
    k3_v = np.zeros((n, 2))
    k3_r = np.zeros((n, 2))
    k3_r_ghost = np.zeros((n, 2))
    for i in range(n):
        k3_r_ghost[i] = positions[i] + (k2_r[i]* dt/2)
    for i in range(n):
        k3_v[i] = TotalAcceleration(i, k3_r_ghost, masses, n)
        k3_r[i] = velocities[i] + (k2_v[i] * dt/2)
    #Calculate k4:
    k4_v = np.zeros((n, 2))
    k4_r = np.zeros((n, 2))
    k4_r_ghost = np.zeros((n, 2))
    for i in range(n):
        k4_r_ghost[i] = positions[i] + (k3_r[i] * dt)
    for i in range(n):
        k4_v[i] = TotalAcceleration(i, k4_r_ghost, masses, n)
        k4_r[i] = velocities[i] + (k3_v[i] * dt)

    new_positions = positions + dt/6*(k1_r+2*k2_r+2*k3_r+k4_r)
    new_velocities = velocities + dt/6*(k1_v+2*k2_v+2*k3_v+k4_v)
    return(new_positions, new_velocities)

#Datos iniciales, S.I.:
G = 6.67430e-11
#Venus:
VENUS_RADIUS = 0
VENUS_ANGULAR_VELOCITY = 0
VENUS_ORBIT = 0
VENUS_MASS = 4.87e24
#Earth:
EARTH_RADIUS = 0
EARTH_ANGULAR_VELOCITY = 0
EARTH_ORBIT = 0
EARTH_MASS = 5.97e24
#Sun:
SUN_RADIUS = 0
SUN_MASS = 1.989e30
#Initial data:
initial_venus_ph = 0
initial_earth_ph = 0
initial_ship_distance = 1e11
initial_ship_velocity = 3e4

Earth = Body("Earth", EARTH_MASS, np.array([1.47e11, 0.0]), np.array([0.0, 30300.0]))
Sun = Body("Sun", SUN_MASS, np.zeros(2), np.zeros(2))
Venus = Body("Venus", VENUS_MASS, np.array([1.082e11, 0.0]), np.array([0.0, 35020.0]))
bodies = [Sun, Earth, Venus]

total_time = 365 #days
dt = 3600 #Seconds
steps = int(total_time*24*3600/dt)

history = np.zeros((steps, len(bodies), 2))

for i in tqdm(range(steps)):
    new_pos, new_vel = GravityEngine(dt, bodies)
    for j in range(len(bodies)):
        history[i, j, :] = new_pos[j]
        bodies[j].vel = new_vel[j]
        bodies[j].pos = new_pos[j]

# --- PLOTTING ---
plt.figure(figsize=(8, 8))

# Loop through bodies one last time to plot their trails
for j, body in enumerate(bodies):
    # Slice the 3D array to get all time steps (:), specific body (j), X coord (0)
    x_coords = history[:, j, 0] 
    # Slice the 3D array to get all time steps (:), specific body (j), Y coord (1)
    y_coords = history[:, j, 1]
    
    plt.plot(x_coords, y_coords, label=body.name)

    # Plot the final position as a dot so we know where they ended up
    plt.plot(x_coords[-1], y_coords[-1], 'o')

plt.title(f"Solar System Orbit ({steps/24:.1f} days)")
plt.xlabel("Distance (meters)")
plt.ylabel("Distance (meters)")
plt.legend()
plt.grid(True)
plt.axis('equal') # Crucial: ensures orbits look circular, not squashed oval
plt.show()