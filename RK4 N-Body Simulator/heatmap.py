import os
import glob
import re
import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime

def generate_heatmap():
    batch_folder = None
    
    #Try to read temp .txt file if it exists
    if os.path.exists("batch_path.txt"):
        with open("batch_path.txt", "r") as f:
            batch_folder = f.read().strip()
    else:
        #If it doesn't search for most recent one
        base_dir = "simulation_results"
        if os.path.exists(base_dir):
            batches = sorted(glob.glob(os.path.join(base_dir, "Batch_*")))
            if batches:
                batch_folder = batches[-1]
                
    if not batch_folder or not os.path.exists(batch_folder):
        print("Error, didn't find a batch folder to process")
        return

    print(f"Reading data from {batch_folder}...")
    
    yaw_list = []
    pitch_list = []
    dist_list = []
    
    info_files = glob.glob(f"{batch_folder}/sim_*/simulation_info_*.txt")
    
    for info_path in info_files:
        with open(info_path, "r") as f:
            content = f.read()
            
            vp_match = re.search(r'Launch Angle:\s*([-+]?[0-9]*\.?[0-9]+)', content)
            pt_match = re.search(r'Pitch Angle:\s*([-+]?[0-9]*\.?[0-9]+)', content)
            dist_match = re.search(r'Minimum Ship Distance:\s*([0-9]+)', content)
            
            if vp_match and pt_match and dist_match:
                yaw_list.append(float(vp_match.group(1)))
                pitch_list.append(float(pt_match.group(1)))
                dist_list.append(float(dist_match.group(1)))
    
    if len(yaw_list) == 0:
        print("No sim data found")
        return
        
    #Get unique values of yaw and pitch to create grid
    unique_yaws = sorted(list(set(yaw_list)))
    unique_pitches = sorted(list(set(pitch_list)))
    
    #Create a matrix filled with NaNs (rows = pitch, cols = yaw)
    Z = np.full((len(unique_pitches), len(unique_yaws)), np.nan)
    
    best_yaw = 0
    best_pitch = 0
    min_dist = float('inf')
    
    #Fill the matrix and find minimum
    for y, p, d in zip(yaw_list, pitch_list, dist_list):
        yaw_idx = unique_yaws.index(y)
        pitch_idx = unique_pitches.index(p)
        Z[pitch_idx][yaw_idx] = d
        
        if d < min_dist:
            min_dist = d
            best_yaw = y
            best_pitch = p
        
    #Create plot
    fig, ax = plt.subplots(figsize=(10, 6))
    ax.set_facecolor('lightgray') # Color para las zonas de colisión (datos faltantes)
    
    Y, X = np.meshgrid(unique_pitches, unique_yaws, indexing='ij')
    c = ax.pcolormesh(X, Y, Z, shading='nearest', cmap='nipy_spectral_r')
    
    fig.colorbar(c, ax=ax, label='Distancia mínima a Venus (m)')
    
    ax.set_xlabel('Guiñada (º)')
    ax.set_ylabel('Cabeceo (º)')
    ax.set_title('Optimización de asistencia gravitatoria')
    
    #Mark the best point
    ax.scatter([best_yaw], [best_pitch], color='black', marker='x', s=5, linewidths=0.5, label=f'Mejor: Guiñada= {best_yaw:.2f}º, Cabeceo= {best_pitch:.2f}º\nDistancia: {min_dist/1000:.0f} km')
    ax.legend(loc='upper center', bbox_to_anchor=(0.5, -0.12))
    
    time_str = datetime.now().strftime("%Y%m%d_%H%M%S")
    output_path_pdf = f"report/graphs/heatmap_{time_str}.pdf"
    output_path_png = f"report/graphs/heatmap_{time_str}.png"
    plt.savefig(output_path_pdf, dpi=400, bbox_inches='tight')
    plt.savefig(output_path_png, dpi=400, bbox_inches='tight')
    plt.close(fig)
    print(f"Heatmaps saved to {output_path_pdf} and {output_path_png}")

    print(f"Best parameters -> Yaw: {best_yaw:.10f}, Pitch: {best_pitch:.10f} (Distance: {min_dist/1000:.0f} km)")

if __name__ == "__main__":
    generate_heatmap()
