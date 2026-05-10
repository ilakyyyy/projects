import os
import glob
import re

# The name of the new combined file
output_file = "results_L_16_128/combined_simulations_L16to128.txt"

# Find all files that match your naming convention
file_pattern = "results_L_16_128/simulations_L*.txt"
files = glob.glob(file_pattern)

with open(output_file, 'w') as outfile:
    # Write the new header with L included
    outfile.write(f"{'L':<8} {'beta':<10} {'energy':<12} {'spec-heat':<12} {'abs-magnet':<12} {'suscept'}\n")
    
    for filename in files:
        # Extract the L number (including decimals) from the filename
        # e.g., "simulations_L16.000.txt" -> "16.000"
        match = re.search(r'simulations_L([\d.]+)\.txt', filename)
        
        if match:
            l_value = match.group(1)
            
            with open(filename, 'r') as infile:
                lines = infile.readlines()
                
                # Skip the first line (the old header) and process the rest
                for line in lines[1:]:
                    line = line.strip()
                    if line:  # Make sure we don't write blank lines
                        # Format it to keep the columns relatively aligned
                        outfile.write(f"{l_value:<8} {line}\n")

print(f"Done! Combined {len(files)} files into {output_file}")