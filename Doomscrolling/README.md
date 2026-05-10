# Prueba 2A: Doomscrolling

## Código principal

El código está organizado de la siguiente forma:

- head.h: común a todos los archivos .c, incluye las definiciones y librerías a usar. No hay que modificarlo, salvo que fuese necesario en algún momento modificar los #define.
- collision.c: funciones dedicadas a establecer si hay una colisión durante la trayectoria.
- game.c: funciones para renderizar el juego usando SDL.
- geometry.c: pequeñas funciones auxiliares que pueden ser útiles para trabajar con vectores.
- input_output.c: funciones para leer/escribir datos.
- movement.c: funciones encargadas de realizar el movimiento de las personas.
- person.c: control de las personas.
- random.c: funciones para generar números aleatorios.
- main.c: la función main que debería llamar a todas las anteriores para simular la evolución del sistema.

## Instalación de SDL3

### macOS

```bash
brew install sdl3 sdl3_ttf
```

### Linux (Debian/Ubuntu)

SDL3 no está disponible aún en los repositorios por defecto. Se debe compilar desde el código fuente:

```bash
# Dependencias
sudo apt install build-essential cmake libfreetype-dev

# SDL3
git clone https://github.com/libsdl-org/SDL.git --branch release-3.2.x --depth 1
cd SDL && cmake -B build && cmake --build build && sudo cmake --install build
cd ..

# SDL3_ttf
git clone https://github.com/libsdl-org/SDL_ttf.git --branch release-3.2.x --depth 1
cd SDL_ttf && cmake -B build && cmake --build build && sudo cmake --install build
cd ..

sudo ldconfig
```

## Tests

No hay que modificar ninguno de los archivos de esta carpeta, pero se puede compilar y ejecutar el archivo public_tests.c para probar si el código pasa los tests antes de subirlo a GitHub.

## Results

En esta carpeta se guardan los resultados de la simulación.