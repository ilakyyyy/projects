import numpy as np
import math
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation, PillowWriter
from tqdm import tqdm
from datetime import datetime
import os

class Body:
    def __init__(self, name, mass, pos, vel):
        self.name = name
        self.mass = mass
        # phase: float
        # radius: float
        # distance: float #(to Sun)
        self.pos = np.array(pos, dtype='float64')
        self.vel = np.array(vel, dtype='float64')

def CalculateHamiltonian(bodies):
    n = len(bodies)
    T = 0.0
    for body in bodies:
        T += 0.5 * body.mass * np.linalg.norm(body.vel)**2
        
    V = 0.0
    for i in range(n):
        for j in range(i + 1, n):
            r = np.linalg.norm(bodies[i].pos - bodies[j].pos)
            if r > 0:
                V += -G*bodies[i].mass*bodies[j].mass/r
                
    return T+V

def ShipHamiltonian(bodies):
    T = 0.5 * bodies[3].mass * np.linalg.norm(bodies[3].vel)**2
    V = 0.0
    for body in bodies:
        r = np.linalg.norm(body.pos-bodies[3].pos)
        if r > 0:
            V += -G*body.mass*bodies[3].mass/r
                
    return T+V

def CalculateSweep(min, max, nsim):
    sweep = np.zeros(nsim)
    rang = abs(max-min)
    if nsim == 1:
        sweep[0] = min + rang/2
    elif nsim > 1:
        step = rang/(nsim-1)
        for i in range(nsim):
            sweep[i] = min + step*i
    
    return(sweep)

def CreateGif(history, bodies, t_vector, output_folder, sim_id, skip, fps, title_text):
    
    # 1. Setup the 2D Figure
    fig, ax = plt.subplots(figsize=(8, 8))
    
    # Calculate fixed bounds so the camera doesn't jitter
    # We use the relative positions (planet - sun)
    all_x = (history[:, :, 0]).flatten()
    all_y = (history[:, :, 1]).flatten()
    
    # Find max distance to center the view
    max_range = (np.max(all_x) - np.min(all_x)) / 2.0
    mid_x = (np.max(all_x) + np.min(all_x)) * 0.5
    mid_y = (np.max(all_y) + np.min(all_y)) * 0.5
    
    # Set limits with a 10% buffer
    buffer = max_range * 0.1
    ax.set_xlim(mid_x - max_range - buffer, mid_x + max_range + buffer)
    ax.set_ylim(mid_y - max_range - buffer, mid_y + max_range + buffer)
    ax.set_aspect('equal') # Crucial for circular orbits
    ax.grid(True, alpha=0.3)
    ax.set_xlabel('X Position (m)')
    ax.set_ylabel('Y Position (m)')
    ax.set_title(title_text, fontsize=10)
    
    # 2. Initialize Lines and Heads
    lines = [] # The trails
    heads = [] # The planet dots
    colors = ['gold', 'blue', 'orange', 'red']
    
    for i, body in enumerate(bodies):
        color = colors[i % len(colors)]
        style = '--' if body.name == "Ship" else '-'
        
        # Trail (line)
        line, = ax.plot([], [], color=color, linestyle=style, alpha=0.6, linewidth=1)
        # Head (dot)
        head, = ax.plot([], [], marker='o', color=color, markersize=8)
        
        lines.append(line)
        heads.append(head)
        
    # Add a time label in the corner
    time_text = ax.text(0.05, 0.95, '', transform=ax.transAxes)

    # 3. The Update Function
    num_frames = len(t_vector) // skip

    def update(frame_idx):
        real_idx = frame_idx * skip
        if real_idx >= len(t_vector): real_idx = len(t_vector) - 1
        
        current_time = t_vector[real_idx]
        time_text.set_text(f"Day: {current_time:.1f}")
        
        for i, body in enumerate(bodies):
            # Get relative coordinates up to this time
            x_data = history[:real_idx, i, 0]
            y_data = history[:real_idx, i, 1]
            
            # Update Trail (Show full history up to now)
            lines[i].set_data(x_data, y_data)
            
            # Update Head (Current position only)
            if len(x_data) > 0:
                heads[i].set_data([x_data[-1]], [y_data[-1]])
                
        return lines + heads + [time_text]

    # 4. Save Animation
    ani = FuncAnimation(fig, update, frames=num_frames, interval=30, blit=True)
    gif_filename = os.path.join(output_folder, f"anim_sim_{sim_id+1}.gif")
    
    # Use PillowWriter (standard in matplotlib)
    ani.save(gif_filename, writer=PillowWriter(fps=fps))
    plt.close(fig)

def GeneratePlots(output_folder, sim_id, history, energy_history, ship_energy_history, bodies, 
                  initial_v_ph, initial_ship_v, min_distance, final_ship_v, 
                  gif, skip, fps, factor):

    # =========================================
    # Plot 1: 2D Orbit Trajectories
    # =========================================
    fig, ax = plt.subplots(figsize=(10, 8)) 

    colors = ['gold', 'blue', 'orange', 'red'] 
    for i, body in enumerate(bodies):
        x = history[:, i, 0]
        y = history[:, i, 1]
        
        color = colors[i % len(colors)]
        style = '--' if body.name == "Ship" else '-'
        width = 1.0 if body.name == "Ship" else 2.0
        alpha = 0.8
        
        # 2D Plot: Only X and Y
        ax.plot(x, y, color=color, linestyle=style, linewidth=width, label=body.name, alpha=alpha)
        # Markers
        ax.plot(x[-1], y[-1], marker='o', color=color, markersize=5) 
        ax.plot(x[0], y[0], marker='.', color=color, markersize=2) 

    ax.set_xlabel('X (m)')
    ax.set_ylabel('Y (m)')
    
    ax.set_title(f"2D trajectories \nInitial venus phase: {round(initial_v_ph, 5)} - Initial ship velocity: {round(initial_ship_v, 3)}\n Final ship velocity: {round(final_ship_v, 3)} m/s - Minimum Distance: {round(min_distance, 0)} m \nFactor: {factor}")
    ax.legend()
    ax.grid(True, alpha=0.3)
    ax.set_aspect('equal') 

    plot_filename = os.path.join(output_folder, f"orbit_plot_sim_{sim_id}.png")
    plt.savefig(plot_filename, dpi=300, bbox_inches='tight')
    plt.close(fig)

    # =========================================
    # Plot 2: Total Hamiltonian
    # =========================================
    fig_en, ax_en = plt.subplots(figsize=(10, 4))
    # Plot percentage change in energy relative to start
    energy_change = (energy_history - energy_history[0]) / np.abs(energy_history[0])
    ax_en.plot(energy_change)
    ax_en.set_title("Total Hamiltonian Stability")
    ax_en.set_xlabel("Time Steps")
    ax_en.set_ylabel("Relative Hamiltonian Change")
    ax_en.grid(True)

    energy_filename = os.path.join(output_folder, f"energy_stability_plot_{sim_id+1}.png")
    plt.savefig(energy_filename, dpi=300, bbox_inches='tight')
    plt.close(fig_en)

    # =========================================
    # Plot 3: Ship Specific Hamiltonian
    # =========================================
    fig_ship, ax_ship = plt.subplots(figsize=(10, 4))
    
    ax_ship.plot(ship_energy_history, color='purple')
    
    ax_ship.set_title(f"Ship Hamiltonian")
    ax_ship.set_xlabel("Time Steps")
    ax_ship.set_ylabel("Energy (Joules)")
    ax_ship.grid(True, alpha=0.3)
    
    ship_energy_filename = os.path.join(output_folder, f"ship_energy_plot_{sim_id+1}.png")
    plt.savefig(ship_energy_filename, dpi=300, bbox_inches='tight')
    plt.close(fig_ship)

    # =========================================
    # Plot 4: GIF
    # =========================================
    if gif == True:
        # We need a t_vector for the GIF. Since we passed everything else, 
        # we can just generate a dummy index vector if strictly needed, 
        # but your CreateGif likely relies on the array length.
        # Ideally, pass t_vector to this function too if CreateGif needs time labels.
        # Assuming history length is enough for now:
        t_vector_dummy = np.arange(len(history)) 
        
        text = f"Initial venus phase: {round(initial_v_ph, 5)} - Initial velocity: {round(initial_ship_v, 3)} - Final velocity: {round(final_ship_v, 3)}\nFactor: {factor}"
        CreateGif(history, bodies, t_vector_dummy, output_folder, sim_id, skip, fps, text)

# Gravity engine:
# It should take everything before and change everything

# First step: Build the acceleration function (positions -> accelerations)
# Second step: 
# Positions: 1 - Body index; 2 - coord

def CalculateAcceleration(target_pos, att_pos, att_mass):
    r = target_pos - att_pos
    norm = np.linalg.norm(r)
    if norm == 0:
        # print(f"Division by 0!")
        return(np.zeros(2))
    else:
        acc = -G*att_mass*r/(norm**3)
        return(acc)

def TensorialTotalAcceleration(positions, masses, n):
    acc_tensor = np.zeros((n, 2))
    for i in range(n):
        for j in range(n):
            acc_tensor[i, :] += CalculateAcceleration(positions[i], positions[j], masses[j])
    return(acc_tensor)

#For one instant of time
def GravityEngine(dt, bodies):
    n = len(bodies)
    # Sun = bodies[0]
    # Earth = bodies[1]
    # Venus = bodies[2]
    positions = np.zeros((n, 2))
    velocities = np.zeros((n, 2))
    masses = np.zeros(n)
    for i in range(n):
        body = bodies[i]
        positions[i, :] = body.pos
        velocities[i, :] = body.vel
        masses[i] = body.mass
    
    # RK-4 Engine:
    # Calculate k1:
    k1_v = np.zeros((n, 2))
    k1_r = np.zeros((n, 2))

    k1_r = velocities
    k1_v = TensorialTotalAcceleration(positions, masses, n)


    # Calculate k2:
    k2_v = np.zeros((n, 2))
    k2_r = np.zeros((n, 2))
    k2_r_ghost = np.zeros((n, 2))
    
    k2_r_ghost = positions + (k1_r * dt/2)
    k2_r = velocities + (k1_v * dt/2)
    k2_v = TensorialTotalAcceleration(k2_r_ghost, masses, n)


    #Calculate k3:
    k3_v = np.zeros((n, 2))
    k3_r = np.zeros((n, 2))
    k3_r_ghost = np.zeros((n, 2))

    k3_r_ghost = positions + (k2_r* dt/2)
    k3_r = velocities + (k2_v * dt/2)
    k3_v = TensorialTotalAcceleration(k3_r_ghost, masses, n)
        

    #Calculate k4:
    k4_v = np.zeros((n, 2))
    k4_r = np.zeros((n, 2))
    k4_r_ghost = np.zeros((n, 2))

    k4_r_ghost = positions + (k3_r * dt)
    k4_r = velocities + (k3_v * dt)
    k4_v = TensorialTotalAcceleration(k4_r_ghost, masses, n)

    #Calculate new positions & velocities:
    new_positions = positions + dt/6*(k1_r+2*k2_r+2*k3_r+k4_r)
    new_velocities = velocities + dt/6*(k1_v+2*k2_v+2*k3_v+k4_v)
    return(new_positions, new_velocities)

def RunSimulation(output_folder, initial_v_ph, initial_ship_v, total_time, factor, sim_id, skip, fps, gif, precision_factor, collision_distance):
    VENUS_R0 = np.linalg.norm(VENUS_R0_P)*np.array([math.cos(math.radians(initial_v_ph)),math.sin(math.radians(initial_v_ph))])
    VENUS_V0 = np.linalg.norm(VENUS_V0_P)*np.array([-math.sin(math.radians(initial_v_ph)),math.cos(math.radians(initial_v_ph))])

    SHIP_V0 = np.array([0.0, initial_ship_v])
    Earth = Body("Earth", EARTH_MASS, EARTH_R0, EARTH_V0)
    Sun = Body("Sun", SUN_MASS, SUN_R0, SUN_V0)
    Venus = Body("Venus", VENUS_MASS, VENUS_R0, VENUS_V0)
    Ship = Body("Ship", SHIP_MASS, SHIP_R0, SHIP_V0)
    bodies = np.array([Sun, Earth, Venus, Ship])


    history_list = []
    energy_history_list = []
    ship_energy_history_list = []
    time_history_list = []
    simulation_time = 0
    max_time = total_time*24*3600
    min_distance = float('inf')
    dt = 3600

    while simulation_time < max_time and min_distance > collision_distance:

        energy_history_list.append(CalculateHamiltonian(bodies))
        ship_energy_history_list.append(ShipHamiltonian(bodies))

        venus_dist = MinimumDistance(bodies, 2)
        dt = factor*(venus_dist)**(1/3)
        if dt > 3600:
            dt = 3600
        if dt < 1:
            dt = 1
        if venus_dist < 1e9:
            dt = dt / precision_factor
        
        new_pos, new_vel = GravityEngine(dt, bodies)
        
        if venus_dist < min_distance:
            min_distance = venus_dist
        
        history_list.append(new_pos)
        time_history_list.append(simulation_time/24/3600)

        for j in range(len(bodies)):
            bodies[j].vel = new_vel[j]
            bodies[j].pos = new_pos[j]
        
        simulation_time += dt

    if min_distance <= collision_distance:
        print(f"\nThere was a collision in simulation {sim_id}")

    history = np.array(history_list)
    energy_history = np.array(energy_history_list)
    ship_energy_history = np.array(ship_energy_history_list)
    t_vector = np.array(time_history_list)

    final_ship_v = np.linalg.norm(bodies[3].vel)
    
    if not os.path.exists(output_folder):
        os.makedirs(output_folder)

    # =========================================
    # Save Data
    # =========================================
    data_filename = os.path.join(output_folder, f"simulation_data_{sim_id+1}.npz")
    np.savez(data_filename, 
            history=history, 
            energy=energy_history, 
            time=t_vector, 
            total_time = max_time,
            venus_initial_phase = initial_v_ph,
            ship_initial_velocity = initial_ship_v,
            body_names=[b.name for b in bodies])
    
    txt_filename = os.path.join(output_folder, f"simulation_info_{sim_id+1}.txt")
    final_energy_error = (energy_history[-1] - energy_history[0]) / np.abs(energy_history[0])
    with open(txt_filename, "w") as f:
        f.write(f"SIMULATION REPORT - ID {sim_id+1}\n")
        f.write("====================================\n\n")
        
        f.write("Initial conditions:\n")
        f.write(f"  Venus Phase:       {initial_v_ph} degrees\n")
        f.write(f"  Ship Velocity:  {initial_ship_v} m/s\n")
        f.write(f"  Bodies:            {[b.name for b in bodies]}\n\n")
        
        f.write("Time settings:\n")
        f.write(f"  Total Steps:        {len(t_vector)}\n")
        
        f.write("Performance:\n")
        f.write(f"  Factor:  {factor}\n")
        f.write(f"  Precision Factor: {precision_factor}\n")
        f.write(f"  Relative Energy Error: {final_energy_error:.5e}\n")
        f.write(f"  Final Ship Velocity: {round(final_ship_v, 0)} m/s\n")
        f.write(f"  Minimum Ship Distance: {round(min_distance, 0)} m\n")


    # =========================================
    # Plot everything
    # =========================================
    GeneratePlots(output_folder, sim_id, history, energy_history, ship_energy_history, 
                  bodies, initial_v_ph, initial_ship_v, min_distance, final_ship_v, 
                  gif, skip, fps, factor)

def MinimumDistance(bodies, index):
    ship_pos = bodies[3].pos
    venus_pos = bodies[index].pos
    return(np.linalg.norm(ship_pos-venus_pos))

#Initial parameters, S.I.:
G = 6.67430e-11
#Venus - perihelion data:
VENUS_RADIUS = 1
VENUS_MASS = 4.87e24
VENUS_R0_P = np.array([1.082e11, 0.0])
VENUS_V0_P = np.array([0.0, 35020.0])
#Earth - initial phase = 0:
EARTH_RADIUS = 1
EARTH_MASS = 5.97e24
EARTH_R0 = np.array([1.47e11, 0.0])
EARTH_V0 = np.array([0.0, 30300.0])
#Sun:
SUN_RADIUS = 1
SUN_MASS = 1.989e30
SUN_R0 = np.zeros(2)
SUN_V0 = np.zeros(2)
#Ship:
SHIP_MASS = 2e4
SHIP_R0 = np.array([1.47e11*(1-1/999), 0])

#Lots of simulations:
# skip = 500
# fps = 6

#Not many simulations - precision:
skip = 150
fps = 20

#3-4 min to compile with dt = 120s, total_t = 3650d:
# skip = 3600
# fps = 30

# Degrees
min_v_phase = -0.008
max_v_phase = -0.006

# m/s
min_s_vel = 25.201667e3
max_s_vel = 25.201667e3

# m
collision_distance = 6.5e6


total_t = 365 #days
#lower values = more precision
factor = 1
#higher values = more precision (will divide dt by it when closer than 10^9km)
precision_factor = 10
total_sims = 1
create_gif = False

output_folder = os.path.join("simulation_results", f"Sim_{datetime.now().strftime('%Y%m%d_%H%M%S')}_n{total_sims}_T{total_t}_f{factor}")

for i in tqdm(range(total_sims)):
    #I just learnt there's a function, np.linspace that does this:
    # venus_phases = CalculateSweep(min_v_phase, max_v_phase, total_sims)
    # ship_velocities = CalculateSweep(min_s_vel, max_s_vel, total_sims)
    venus_phases = np.linspace(min_v_phase, max_v_phase, total_sims)
    ship_velocities = np.linspace(min_s_vel, max_s_vel, total_sims)

    RunSimulation(output_folder, venus_phases[i], ship_velocities[i], total_t, factor, i, skip, fps, create_gif, precision_factor, collision_distance)


#Future upgrades:
#   if v > something, just stop the simulation (don't let it end)
#   we need to prevent planet collisions (or know when one has happened)