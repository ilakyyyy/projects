#include "head.h"

/**
 * Count the number of walls in the file.
 */
int countWalls(const char *filename) {
    float x1, y1, x2, y2;
    int count;

    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file %s\n", filename);
        return -1;
    }

    count = 0;
    while(fscanf(file, "%f %f %f %f\n", &x1, &y1, &x2, &y2) == 4) {
        count++;
    }

    fclose(file);

    return count;
}

/**
 * Load the walls from the file into an array of segments.
 */
Segment* loadWalls(const char *filename, int wall_count) {
    float max_x, max_y;

    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file %s\n", filename);
        return NULL;
    }

    Segment *walls = malloc(wall_count * sizeof(Segment));

    for(int i = 0; i < wall_count; i++) {
        fscanf(file, "%f %f %f %f\n", &walls[i].start.x, &walls[i].start.y, &walls[i].end.x, &walls[i].end.y);
    }

    fclose(file);

    return walls;
}

/**
 * Load the professor locations from the file into an array of persons.
 */
void loadProfessors(Level *level) {
    FILE *file = fopen("data/professor_spawn.txt", "r");
    if (!file) {
        printf("Error opening file %s\n", "data/professor_spawn.txt");
        return;
    }

    fscanf(file, "%d", &level->n_professors);
    level->professors = malloc(level->n_professors * sizeof(Person));

    for(int i = 0; i < level->n_professors; i++) {
        fscanf(file, "%f %f\n", &level->professors[i].position.x, &level->professors[i].position.y);
    }

    fclose(file);
}

/**
 * Load the level data including walls and parameters.
 */
void loadData(Level *level) {
    level->n_walls = countWalls("data/coordinates.txt");
    level->walls = loadWalls("data/coordinates.txt", level->n_walls);

    loadProfessors(level);
    level->center = (Point) {.x = WINDOW_WIDTH/2, .y = WINDOW_HEIGHT/2};
}

/**
 * Stores the results of the simulation in a file.
 * The file is named "results/person_<student_nip>.txt".
 */
void storeResults(Level level, Person student, int duration) {
    char filename[STRING_LENGTH];
    FILE *file;

    sprintf(filename, "results/student_%d_%d.txt", STUDENT_NIP, student.detection_shape);

    file = fopen(filename, "w");
    fprintf(file, "Student NIP: %d\n", STUDENT_NIP);
    fprintf(file, "Questions asked: %d\n", student.log.questions_asked);
    fprintf(file, "Distance: %.2f\n", student.log.distance);
    fprintf(file, "Professors left: %d\n", level.n_professors);
    fprintf(file, "Simulation duration: %d\n", duration);
    fprintf(file, "Current stress: %.2f%%\n", student.panic.current_stress);
    fclose(file);

    printf("Results stored in %s\n", filename);
}