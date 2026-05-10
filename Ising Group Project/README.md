# Modelo de Ising

## Uso de GitHub en grupo
Si dos personas editan exactamente el mismo archivo al mismo tiempo, Git provocará un *merge conflict* (que se puede resolver).

1. **Pull (Sincronizar):** Antes de empezar a trabajar, *siempre* descarga (haz pull) de los últimos cambios que hayan hecho tus compañeros. 
   * Comando: `git pull`
   * En la interfaz (a la izquierda): id a "source control" (los 3 bolitos unidos) y dadle al botón grande
        azul ("Sync changes"); si no os sale nada, dadle al botón "Synchronize changes" en el apartado de "Repositories"
2. **Trabajar:** Haz los cambios necesarios en los archivos que quieras.
3. **Commit (Guardar localmente):** Guarda tus cambios con un mensaje corto y descriptivo sobre lo que hiciste.
   * Comando: `git add .` (para preparar tus archivos)
   * Comando: `git commit -m "Añadidas las funciones de la energía y magnetización"`
   * En la interfaz (a la izquierda): id a "source control" y en la pestaña de "Changes", dadle al "+" (Stage All Changes), después
        escribid un mensaje donde pone "Message" (si no escribís nada, se os abrirá una pestaña en la que os pide un mensaje), y después
        dadle al botón azul (Commit), y luego a "Sync changes" $-$ esto último es equivalente al siguiente punto (el push)
4. **Push (Compartir):** Sube tus cambios a GitHub para que el resto de tu equipo pueda verlos.
   * Comando: `git push`

## Estructura de desarrollo propuesta

### Primera semana

Objetivos: ser capaces de generar configuraciones y medir la energía y magnetización de una.

1) Generación de configuraciones. Funciones a implementar en `utils.c`:

    - `generate_random_configuration`
    - `generate_all_up_configuration`
    - `generate_all_down_configuration`
    - `generate_chess_board_configuration`
    - `generate_configuration`

Una vez implementadas, se puede utilizar el main del archivo `utils.c` y el *run and debug* `(1) Debug utils.c` para comprobar que sean correctas. No hace falta entender los *malloc*, con entender la variable `configuration_flag` es suficiente.

2) Medir la energía y magnetización de una configuración. Funciones a implementar en `observables.c`:

    - `build_direction_vectors`
    - `calculate_energy`
    - `calculate_magnetization`

Una vez implementadas, se puede utilizar el *main* del archivo `observables.c` y el *run and debug* `(2) Debug observables.c` para comprobar que sean correctas. La idea sería medir la energía y la magnetización para distintas configuraciones y razonar si tienen sentido. No hace falta entender todo el contenido del *main*, con entender la variable `configuration_flag` es suficiente.

3) Extra: guardar la configuración en un archivo y pintarla con gnuplot. Funciones a implementar en `io.c`:

    - `save_configuration`

Una vez implementada, ejecutar el *run and debug* `(1) Debug utils.c` y utilizar el script de gnuplot de la carpeta `scripts` para generar la imagen de la configuración. Se recomienda abrir el script.

### Segunda semana

Objetivos: ser capaces de generar N configuraciones, medir su energía y magnetización y representarlas en un histograma.

1) Se pide implementar una versión simplificada de 

    - `measure_observables`

en `metropolis.c` la cual se completará en la siguiente semana. Simplemente mediremos la energía y la magnetización *n_measurements* veces, regenerando la configuración entre cada medida (es decir, llamando a `generate_configuration`).

2) Importar las funciones de análisis en `analysis.c`:

    - `med_var`
    - `histogram`

Una vez adaptadas, se puede utilizar el *main* del archivo `analysis.c` y el *run and debug* `(3) Debug analysis.c` para comprobar que todo esté correcto. Habrá que comparar lo que salen las distintas configuraciones con el Cuadro 10.3, para varios valores de *L* y dibujar algunos histogramas representativos.

3) Extra: crear scripts de gnuplot que nos faciliten la generación de imágenes.

### Tercera semana

Objetivos: implementar el algoritmo de Metropolis para explorar las configuraciones compatibles con una temperatura dada.

1) Generar configuraciones aleatorias para una temperatura dada. Para ello, implementar las funciones:

    - `initialize_probability_array`
    - `metropolis_update`

de `metropolis.c`. A continuación, modificar la función `measure_observables` realizada la semana anterior para que, en lugar de generarse una configuración nueva cada vez, se generen `n_metropolis_steps` con la función `metropolis_update` entre cada medida. 

Para comprobar que todo es correcto, dar un valor a *beta* y usar el *run and debug* `(4) Debug metropolis.c` para medir la configuración. Cambiar los parámetros y ver que todo sigue lo esperado.

2) Introducir la lógica necesaria en la función *main* del archivo `ising.c` para hacer las simulaciones del modelo de Ising en función de la temperatura (beta). Para ello, se recomienda estudiar la estructura de la sección 11.3, así como la función *main* del archivo `metropolis.c`.

Se puede compilar el código final con el *run and debug* `(5) Run the simulation`.

3) Extra:
    - Usar la función `sprintf` para que los archivos de resultados incluyan datos sobre la simulación (L, medidas...).
    - Crear scripts para faciltiar la generación de figuras.
    - Crea una función `read_input` en `io.c` que lea los parámetros de la simulación de archivos contenidos en *data*.

## Uso de GitHub en grupo

Trabajar en un equipo de seis implica coordinarse para no sobrescribir el trabajo de los demás. En Git, hay muchas formas de colaborar. Proponemos dos métodos: el **Modo Fácil** (ideal para empezar rápido) y el **Modo Avanzado** (como trabajan los equipos profesionales). 

Se incluyen también los comandos que habría que escribir en la terminal, pero se puede hacer lo mismo con los botones del VS Code.

### Método 1: El flujo directo (Modo Fácil)

Esta es la forma más sencilla de trabajar juntos, pero es importante coordinarse con el equipo para asegurarse de que todos estáis trabajando en *archivos completamente diferentes*. Si dos personas editan exactamente el mismo archivo al mismo tiempo, Git se confundirá y provocará un *merge conflict* (que se puede resolver).

#### Los Pasos:
1. **Pull (Sincronizar):** Antes de empezar a trabajar, *siempre* descarga (haz pull) de los últimos cambios que hayan hecho tus compañeros. 
   * Comando: `git pull`
2. **Trabajar:** Haz los cambios necesarios en los archivos que te tocaron.
3. **Commit (Guardar localmente):** Guarda tus cambios con un mensaje corto y descriptivo sobre lo que hiciste.
   * Comando: `git add .` (para preparar tus archivos)
   * Comando: `git commit -m "Añadidas las funciones de la energía y magnetización"`
4. **Push (Compartir):** Sube tus cambios a GitHub para que el resto de tu equipo pueda verlos.
   * Comando: `git push`

> **Ventajas:** Muy rápido, fácil de aprender y evita comandos complejos de Git.

> **Desventajas:** Si accidentalmente editas el mismo archivo que un compañero, te enfrentarás a conflictos.

---

### Método 2: El flujo de Ramas (Modo Avanzado)

Así es como trabajan los equipos de desarrollo profesionales. En lugar de que todos editen la rama principal directamente, creas una "rama" (branch), que es una copia segura e independiente del código, para trabajar en tu parte específica del proyecto. 

*Nota: Como es la primera vez usando este flujo, cuidado. Si se hacen cambios en muchos archivos dentro de una rama, integrarla de nuevo en el proyecto principal será muy difícil.*

#### Los Pasos:
1. **Branch (Crea tu espacio de trabajo):** Crea una nueva rama para la tarea específica en la que estás trabajando y cámbiate a ella.
   * Comando: `git checkout -b tu-nombre/nombre-tarea` (ej. `git checkout -b sara/energia`)
2. **Trabajar y Commit:** Al igual que en el Método 1, haz tus cambios y guárdalos (commit) localmente.
   * Comandos: `git add .` y luego `git commit -m "Añadida la función energía"`
3. **Push a tu Rama:** Sube tu rama específica a GitHub.
   * Comando: `git push origin tu-nombre/nombre-tarea`
4. **Abrir un Pull Request (PR):** Ve a tu repositorio en la página de GitHub. Verás un botón verde que dice "Compare & pull request". Esto le pide al equipo que revise e integre tu rama en la rama `main`.
5. **Merge (Fusionar):** Alguien de tu equipo revisa el Pull Request y hace clic en el botón verde "Merge pull request" en GitHub para finalizar y añadir tu código al proyecto principal. 

> **Ventajas:** Protege el proyecto principal (`main`) de errores; te permite experimentar de forma segura.

> **Desventajas:** Requiere más pasos. Si esperas demasiado para fusionar o tocas demasiados archivos sin avisar, el Pull Request será difícil de integrar limpiamente. 

---

### Consejos

* Cread un chat grupal (WhatsApp, Discord, etc.). Antes de empezar a trabajar en un archivo, avisad: *"Voy a trabajar en el archivo `observables.c` ahora mismo."*
* **Haz Pull antes de hacer Push.** Siempre baja el código más reciente de tus compañeros antes de intentar subir el tuyo.
* **Haz Commits a menudo.** No esperes hasta el final de la semana para guardar. Haz commits y push de pequeños bloques de trabajo (ej. "Terminada la energía", "Terminada la magnetización").