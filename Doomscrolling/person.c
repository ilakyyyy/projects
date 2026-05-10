#include "head.h"

#ifndef __SDL__
// Fix for working without SDL, you can ignore this part
#define Uint64 unsigned long long
double updateTime(Uint64 *lastTime) {
    return 1.0;
}
#endif

/**
 * Initialize the student.
 */
void setUpStudent(Person *student) {
    FILE *file;
    char filename[STRING_LENGTH], buffer[STRING_LENGTH];

    // Initialize basic variables
    student->position = (Point) {.x = WINDOW_WIDTH/2, .y = WINDOW_HEIGHT/2};
    student->status = SEARCHING;
    student->panic = (Panic) {
        .current_stress = 0,
        .maximum_stress = 100,
        .stress_recovery_rate = 10,
        .levels = STRESS_LEVELS
    };

    student->battery = (Battery) {
        .current_charge = 100,
        .maximum_charge = 100,
        .levels = BATTERY_LEVELS
    };

    student->log = (Log) {
        .questions_asked = 0,
        .distance = 0
    };

    student->target_idx = -1;

    // Read student characteristics from file
    sprintf(filename, "data/student.txt");
    file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file %s\n", filename);
        return;
    }

    fscanf(file, "%s %f", buffer, &student->max_step);
    fscanf(file, "%s %f", buffer, &student->velocity);
    fscanf(file, "%s %d", buffer, &student->radius);
    fscanf(file, "%s %f", buffer, &student->panic.stress_searching_rate);
    fscanf(file, "%s %f", buffer, &student->panic.stress_chasing_rate);
    fscanf(file, "%s %d", buffer, &student->color.r);
    fscanf(file, "%s %d", buffer, &student->color.g);
    fscanf(file, "%s %d", buffer, &student->color.b);
    fscanf(file, "%s %d", buffer, &student->color.a);

    fclose(file);

    // Initial conditions
    student->step_size = 0;
    student->angle = 0;
    student->delta_angle = 0;

    student->detection_shape = CONE;
    student->detection_half_angle = DETECTION_HALF_ANGLE;
    student->detection_width = DETECTION_WIDTH;
        
    Point destination = newDestination(student);
    updateDestination(student, destination);
}

/**
 * Read professor properties from file and assign them to each professor
 * already loaded in the level. Professors share the same base properties
 * but start at different positions (read by loadProfessors).
 */
void setUpProfessors(Level *level) {
    FILE *file;
    char buffer[STRING_LENGTH];
    float max_step, velocity;
    int radius;
    Color color;

    file = fopen("data/professor.txt", "r");
    if (!file) {
        printf("Error opening file data/professor.txt\n");
        return;
    }

    fscanf(file, "%s %f", buffer, &max_step);
    fscanf(file, "%s %f", buffer, &velocity);
    fscanf(file, "%s %d", buffer, &radius);
    fscanf(file, "%s %d", buffer, &color.r);
    fscanf(file, "%s %d", buffer, &color.g);
    fscanf(file, "%s %d", buffer, &color.b);
    fscanf(file, "%s %d", buffer, &color.a);

    fclose(file);

    for (int i = 0; i < level->n_professors; i++) {
        Person *prof = &level->professors[i];
        prof->max_step = max_step;
        prof->velocity = velocity;
        prof->radius = radius;
        prof->color = color;

        prof->step_size = 0;
        prof->angle = uniformRandomIn(-M_PI, M_PI);
        prof->delta_angle = 0;

        Point destination = newDestination(prof);
        updateDestination(prof, destination);
    }
}



/**
 * No-op.
 */
void updateDetectionArea(Person *student) {
    (void)student;
}

/**
 * Returns true if the professor is within the student's detection area.
 */
bool professorInRange(Person student, Point professor_position) {
    Point rel = substractVectors(professor_position, student.position);
    float dist = norm(rel);

    switch (student.detection_shape) {
        case CIRCLE:
            return dist < DETECTION_RADIUS;

        case CONE: {
            if (dist >= DETECTION_RADIUS || dist == 0.0f) return false; 
            
            float dot_product = (rel.x * student.direction.x) + (rel.y * student.direction.y);
            float cos_angle = dot_product / dist; 
            
            return cos_angle > cosf(student.detection_half_angle);
        }
    }
    return false;
}

/**
 * Moves all professors randomly using the same movement functions as the student.
 */
void moveProfessors(Level *level, float dt) {
    for (int i = 0; i < level->n_professors; i++) {
        Person *prof = &level->professors[i];
        bool success = movePerson(prof, *level, dt);
        if (!success) {
            updateDestination(prof, newDestination(prof));
        }
    }
}

int findProfessor(Person student, Level level) {
    int *close_professors;
    int cnt = 0;
    close_professors = NULL;

    //Professors within range search
    for(int i=0; i< level.n_professors; i++) {
        if (professorInRange(student, level.professors[i].position) 
            && (WALLHACK_ALLOWED || allowedMovement(student.position, level.professors[i].position, student.radius, level))
        ) 
        {
            int *temp = realloc(close_professors, (cnt+1)*sizeof(int));
            if (temp != NULL) {
                close_professors = temp;
                *(close_professors+cnt) = i;
                cnt++;
            }
            else {
                printf("Critical error, realloc returned NULL. cnt = %d\n", cnt);
                free(close_professors);
                return -1;
            }
        }
    }
    //Return -1 if none have been found
    if (cnt == 0) return(-1);

    //Closest professor idx IN CLOSE_PROFESSORS search and return
    int closest = 0;
    float closest_dist = distancePoints(student.position, level.professors[ close_professors[0] ].position);
    for(int i=0; i<cnt; i++) {
        if (distancePoints(student.position, level.professors[ close_professors[i] ].position) < closest_dist) {
            closest = i;
            closest_dist = distancePoints(student.position, level.professors[ close_professors[i] ].position);
        }
    }
    int idx = close_professors[closest];
    free(close_professors);
    return(idx);
}

/**
 * Removes a professor from the array by replacing it with the last one.
 */
//I'll assume index is between 0 and level->n_professors-1
void removeProfessor(Level *level, int index) {
    level->professors[index] = level->professors[level->n_professors-1];
    level->n_professors--;
}

/**
 * Returns the battery drain rate per second for the student's detection shape.
 */
static float batteryDrainRate(Person student) {
    switch (student.detection_shape) {
        case CIRCLE: return BATTERY_DRAIN_CIRCLE;
        case CONE: return BATTERY_DRAIN_CIRCLE * student.detection_half_angle / (M_PI * 2);
    }
    return BATTERY_DRAIN_CIRCLE;
}

/**
 * Decreases the battery charge over dt seconds based on the detection shape.
 * If the battery reaches 0, the student's stress is set to maximum.
 */
void updateBattery(Person *student, float dt) {
    student->battery.current_charge -= batteryDrainRate(*student) * dt;

    if (student->battery.current_charge <= 0) {
        student->battery.current_charge = 0;
        student->panic.current_stress = student->panic.maximum_stress;
    }

    student->battery.levels = (int)ceil(
        student->battery.current_charge / student->battery.maximum_charge * BATTERY_LEVELS);
}

/**
 * Increases the stress of the person in the interval dt and
 * updates the level of panic.
 */
void updatePanic(Person *student, float dt) {
    float max_stress = student->panic.maximum_stress;
    float current_stress = student->panic.current_stress;
    float new_stress;
    switch (student->status) {
        case SEARCHING:
        new_stress = current_stress + student->panic.stress_searching_rate*dt;

        break;
        case CHASING:
        new_stress = current_stress + student->panic.stress_chasing_rate*dt;
        
        break;
        case ASKING:
        new_stress = current_stress - student->panic.stress_recovery_rate*dt;

        break;
    }
    if (new_stress > max_stress) new_stress = max_stress;
    if (new_stress < 0) new_stress = 0;
    // printf("edu maricon -- updatePanic function call; new stress: %f%%\n", new_stress);

    student->panic.current_stress = new_stress;
    int stress_level = floor(student->panic.current_stress / max_stress * STRESS_LEVELS);
    student->panic.levels = stress_level;
}

void hacks1(Level *level, Person *student, int *hack) {
    *hack = level->n_walls;
    *(hack+1) = student->velocity;
    level->n_walls = 0;
    student->velocity = 1200;
    if (student->position.x > WINDOW_WIDTH || student->position.x < 0) {
        student->position.x = WINDOW_WIDTH/2.0;
        updateDestination(student, newDestination(student));
    }
    if (student->position.y > WINDOW_HEIGHT || student->position.y < 0) {
        student->position.y = WINDOW_HEIGHT/2.0;
        updateDestination(student, newDestination(student));
    }
}

void hacks2(Level *level, Person *student, int *hack) {
    level->n_walls = *hack;
    student->velocity = *(hack+1);
}

/**
 * Runs all the dynamics in the system.
 */
void runDynamics(Level *level, Person *student, Uint64 *lastTime, int *hack) {
    if (PHASE_ALLOWED) hacks1(level, student, hack);
    float dt = updateTime(lastTime);
    // printf("edu maricon -- calculated dt: %f\n", dt);
    switch (student->status) {
        case ASKING:
        // printf("edu maricon -- asking\n");
        updateDestination(student, level->professors[ student->target_idx ].position);
        if (student->panic.current_stress < 0.01) {
            removeProfessor(level, student->target_idx);
            student->target_idx = -1;
            student->log.questions_asked++;
            student->status = SEARCHING;
        }

        break;
        case SEARCHING:
        // printf("edu maricon -- searching\n");

        int idx = findProfessor(*student, *level);
        if (idx != -1) {
            student->status = CHASING;
            student->target_idx = idx;
        }
        
        break;
        case CHASING:
        // printf("edu maricon -- chasing\n");
        if (professorInRange(*student, level->professors[ student->target_idx ].position)) {
            updateDestination(student, level->professors[ student->target_idx ].position);
            if (distancePoints(student->position, level->professors[ student->target_idx ].position) < ASKING_RADIUS) {
                student->status = ASKING;
            }

            //Camping prevention:
            if (allowedMovement(student->position, student->destination, student->radius, *level)) {
                student->camping_timer = 0;
            }
            else {
                student->camping_timer += dt;
            }

            if (student->camping_timer >= MAX_CAMPING) {
                student->camping_timer = 0;
                student->status = SEARCHING;
                student->target_idx = -1;
                updateDestination(student, newDestination(student));
            }
        }
        else {
            student->status = SEARCHING;
            student->target_idx = -1;
            updateDestination(student, newDestination(student));
        }

        break;
    }

    if (!movePerson(student, *level, dt)){
        updateDestination(student, newDestination(student));
        // printf("edu maricon -- entrado en if\n");
    }
    if (PHASE_ALLOWED) hacks2(level, student, hack);
    moveProfessors(level, dt);
    updatePanic(student, dt);
    updateBattery(student, dt);
    // printf("edu maricon -- pasado bucle, panic: %f%%\n", student->panic.current_stress);
}

