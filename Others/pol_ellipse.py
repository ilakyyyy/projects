import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# 1. Configuración de la luz (Elipse fija en el espacio)
A1, A2 = 1.0, 0.6
delta_init = np.deg2rad(45)  # Desfase inicial en base XY
t = np.linspace(0, 2*np.pi, 200)

# Campos originales (estos no cambian, son la "realidad")
Ex_base = A1 * np.cos(t)
Ey_base = A2 * np.cos(t + delta_init)

# Configuración de la figura
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))

# --- Preparación Panel 1 (Elipse y Ejes) ---
ax1.plot(Ex_base, Ey_base, color='gray', alpha=0.3, label='Trayectoria Luz')
line_x_axis, = ax1.plot([], [], 'r-', lw=2, label="Eje X' (Cabeza)")
line_y_axis, = ax1.plot([], [], 'g-', lw=2, label="Eje Y'")
ax1.set_xlim(-1.5, 1.5); ax1.set_ylim(-1.5, 1.5)
ax1.set_aspect('equal')
ax1.legend(loc='upper right')

# --- Preparación Panel 2 (Ondas Proyectadas) ---
wave_x, = ax2.plot([], [], 'r-', lw=2, label="Onda en X'")
wave_y, = ax2.plot([], [], 'g-', lw=2, label="Onda en Y'")
text_delta = ax2.text(0.05, 0.9, '', transform=ax2.transAxes, fontsize=12, fontweight='bold')
ax2.set_xlim(0, 2*np.pi); ax2.set_ylim(-1.5, 1.5)
ax2.set_title("Desfase detectado por tus ojos")
ax2.legend(loc='upper right')

def update(psi_deg):
    psi = np.deg2rad(psi_deg)
    
    # "Girar la cabeza": Proyectamos la luz en los nuevos ejes rotados
    # Basado en la matriz de rotación de tus apuntes (pág. 8)
    Ex_rot = Ex_base * np.cos(psi) + Ey_base * np.sin(psi)
    Ey_rot = -Ex_base * np.sin(psi) + Ey_base * np.cos(psi)
    
    # Actualizar dibujo de ejes
    line_x_axis.set_data([0, 1.5*np.cos(psi)], [0, 1.5*np.sin(psi)])
    line_y_axis.set_data([0, -1.5*np.sin(psi)], [0, 1.5*np.cos(psi)])
    
    # Actualizar ondas
    wave_x.set_data(t, Ex_rot)
    wave_y.set_data(t, Ey_rot)
    
    # Calcular el desfase numérico (usando la diferencia de tiempos de los picos)
    # Esto es lo que el formalismo de Jones captura matemáticamente
    phase_x = np.angle(np.max(Ex_rot) + 0j) # Simplificación visual
    # Usamos la fase compleja para ser exactos
    Ex_c = A1 * np.cos(psi) + (A2 * np.exp(1j*delta_init)) * np.sin(psi)
    Ey_c = -A1 * np.sin(psi) + (A2 * np.exp(1j*delta_init)) * np.cos(psi)
    current_delta = np.rad2deg(np.angle(Ey_c) - np.angle(Ex_c))
    
    text_delta.set_text(f"Giro cabeza: {psi_deg}°\nDesfase Delta: {current_delta:.1f}°")
    
    return line_x_axis, line_y_axis, wave_x, wave_y, text_delta

# Crear la animación (360 grados de giro)
ani = FuncAnimation(fig, update, frames=np.arange(0, 360, 2), interval=50, blit=True)

plt.tight_layout()
plt.show()