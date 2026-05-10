import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import tqdm

# Set-up parametrs
L = 128
B = [0.600, 0.700, 0.441, 0.442, 0.399, 0.440, 0.443, 0.000, 0.200, 0.300, 1.000]
for valor in B:
    filename = f"results/spins_gif_L{L:03}_B{valor:.3f}.bin"

    with open(filename, "rb") as f:
        data = np.fromfile(f, dtype=np.int8) #continuous stream of +-1
    
    total_frames = len(data)//(L*L)

    complete_frames = data[:total_frames*L**2].reshape(-1,L,L) #-1 tells np "u'll figure it out"
    # safety crop included here |^|

    culling_threshold = int(L/(4))
    frames = np.concatenate([
        complete_frames[:culling_threshold],
        complete_frames[culling_threshold::10]
    ])

    '''.txt reading (obsolete)'''
    # # Reading data from file
    # # We read each line, convert to int, and reshape to (L, L)
    # filename = f"results/spins_gif_L{L:03}_B{valor:.3f}.txt"
    # frames = []
    # culling = 1
    # exp_size = L*L
    # with open(filename, 'r') as f:
    #     for i, line in enumerate(tqdm.tqdm(f, desc=f"Loading B={valor:.3f}")):
    #         if i > L/(4*6):
    #             culling = 10
            
    #         if i%culling == 0:
    #             try:
    #                 # Convert each line (containing the +1 and -1's) to a L*L np matrix
    #                 state = np.fromstring(line.strip(), sep=' ', dtype=int)
    #                 if state.size == exp_size:
    #                     frames.append(state.reshape((L, L)))
    #                 else:
    #                     print(f"\n[Warning] Line {i} was incomplete. Got {state.size}/{exp_size}. Skipping.")
    #             except ValueError:
    #                 print(f"\n[Error] Line {i} is corrupted. Skipping.")
    #                 continue

    # Create animation
    fig = plt.figure(figsize=(5.12, 5.12), dpi=100)
    ax = fig.add_axes([0, 0, 1, 1])
    ax.set_axis_off()
    im = ax.imshow(frames[0], cmap='binary', interpolation='nearest', animated=True)

    def update(frame_idx): #Function for updating a frame
        im.set_array(frames[frame_idx])
        return [im]

    ani = animation.FuncAnimation(fig, update, frames=tqdm.tqdm(range(len(frames)), desc=f"Rendering B={valor:.3f}"), interval=100, blit=True) #interval is in ms (50 \implies 20fps)

    # Save as GIF
    ani.save(f"plots/ising_thermalization_L{L:03}_B{valor:.3f}_final_slowed_at_first_binary.gif", writer='pillow')
    plt.close()