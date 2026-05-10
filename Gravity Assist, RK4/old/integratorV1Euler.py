import numpy as np
import math
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

def Acceleration(Planet, Ship):
    r = Ship.pos-Planet.pos
    norma = np.linalg.norm(r)
    if norma == 0:
        print(f"Division by 0! (When calculating the acceleration {Planet} \"makes\" over {Ship})")
        return(np.zeros(2))
    acc = -G*Planet.mass/(norma**3) * r
    return(acc)

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
dt = 360 #Seconds
steps = int(total_time*24*3600/dt)

history = np.zeros((steps, len(bodies), 2))

for i in tqdm(range(steps)):

    accelerations = {}
    for body in bodies:
        total_acc = np.zeros(2)
        for other_body in bodies:
            if other_body == body:
                continue
            else:
                acc = Acceleration(other_body, body)
                total_acc += acc
        accelerations[body.name] = total_acc


    for j in range(len(bodies)):
        acc = accelerations[bodies[j].name]
        bodies[j].vel = bodies[j].vel + dt*acc
        bodies[j].pos = bodies[j].pos + dt*bodies[j].vel
        history[i, j, :] = bodies[j].pos

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

plt.title(f"Solar System Orbit ({total_time} days)")
plt.xlabel("Distance (meters)")
plt.ylabel("Distance (meters)")
plt.legend()
plt.grid(True)
plt.axis('equal') # Crucial: ensures orbits look circular, not squashed oval
plt.show()