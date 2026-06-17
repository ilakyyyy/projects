import os
import sys
import subprocess
import re
from datetime import datetime

def compile_and_run_c():
    print("Compiling C code...")
    compile_cmd = ["gcc", "simulation.c", "functions.c", "RK4integrator.c", "-o", "sim.exe", "-O3", "-lm", "-fopenmp"]
    try:
        subprocess.run(compile_cmd, check=True)
    except subprocess.CalledProcessError:
        print("Failed to compile C code")
        sys.exit(1)
        
    #Dynamically find 'total_sims' from the C source code
    with open("simulation.c", "r") as f:
        content = f.read()
        match_yaw = re.search(r'int\s+yaw_sims\s*=\s*(\d+);', content)
        match_pitch = re.search(r'int\s+pitch_sims\s*=\s*(\d+);', content)
        
        if match_yaw and match_pitch:
            total_sims = int(match_yaw.group(1)) * int(match_pitch.group(1))
        else:
            match_total = re.search(r'int\s+total_sims\s*=\s*(\d+);', content)
            total_sims = int(match_total.group(1)) if match_total else 1
    
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    batch_folder = f"simulation_results/Batch_{timestamp}"
    
    print(f"Preparing {total_sims} simulations in {batch_folder}...")
    for i in range(total_sims):
        os.makedirs(f"{batch_folder}/sim_{i+1}", exist_ok=True)
        
    #C communication via .txt
    with open("batch_path.txt", "w") as f:
        f.write(batch_folder)
        
    #Run cross-platform compatible executable
    print("Initializing C simulation...")
    exe_name = "sim.exe" if sys.platform == "win32" else "./sim.exe"
    subprocess.run([exe_name], check=True)
    
    return total_sims, batch_folder

if __name__ == "__main__":
    total_simulations_run, batch_folder = compile_and_run_c()
    
    #Clean up the temp file
    if os.path.exists("batch_path.txt"):
        os.remove("batch_path.txt")
        
    print(f"\nSuccesfully completed {total_simulations_run}. Data saved in: {batch_folder}")