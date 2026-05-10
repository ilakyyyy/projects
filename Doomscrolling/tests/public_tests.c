#include "../head.h"
#include <stdbool.h>
#include <math.h>
#include <string.h>

extern bool intersectionTest(double numerator, double denominator);
extern bool segmentsIntersect(Segment s1, Segment s2);
extern float squaredDistancePointSegment(Point a, Point b, Point c);
extern bool circleCollision(Point p_end, int radius, Segment wall);
extern bool allowedMovement(Point p_start, Point p_end, int radius, Level level);

extern void updatePanic(Person *student, float dt);
extern int findProfessor(Person student, Level level);
extern void removeProfessor(Level *level, int index);
extern void moveProfessors(Level *level, float dt);

#define RUN_TEST(test_function) do { \
    n_test++; \
    printf("\t%d) ", n_test); \
    status = test_function(); \
    if(status) { \
        printf("OK\n"); \
        passed++; \
    } \
} while(0);

#define EPSILON 1e-3
#define APPROX_EQUAL(x, y) (fabs((x) - (y)) < EPSILON)

/* ==================== geometry.c tests ==================== */

bool test_substractVectors() {
    Point p1 = {1, 2};
    Point p2 = {4, 5};
    Point result;

    printf("Testing substractVectors...");

    result = substractVectors(p1, p2);
    if(result.x != -3 || result.y != -3) {
        printf("\n\tERROR: Wrong result, expected (-3, -3), obtained (%.1f, %.1f)\n", result.x, result.y);
        return false;
    }

    p1.x = 0;
    p1.y = 0;
    result = substractVectors(p1, p2);
    if(result.x != -4 || result.y != -5) {
        printf("\n\tERROR: Wrong result, expected (-4, -5), obtained (%.1f, %.1f)\n", result.x, result.y);
        return false;
    }

    return true;
}

bool test_dotProduct() {
    Point p1 = {1, 2};
    Point p2 = {4, 5};
    float result;

    printf("Testing dotProduct...");

    result = dotProduct(p1, p2);
    if(result != 14) {
        printf("\n\tERROR: Wrong result, expected 14, obtained %.1f\n", result);
        return false;
    }

    p1.x = 0;
    p1.y = 0;
    result = dotProduct(p1, p2);
    if(result != 0) {
        printf("\n\tERROR: Wrong result, expected 0, obtained %.1f\n", result);
        return false;
    }

    return true;
}

bool test_scalarProduct() {
    Point p = {1, 2};
    float scalar = 3;
    Point result;

    printf("Testing scalarProduct...");

    result = scalarProduct(p, scalar);
    if(result.x != 3 || result.y != 6) {
        printf("\n\tERROR: Wrong result, expected (3, 6), obtained (%.1f, %.1f)\n", result.x, result.y);
        return false;
    }

    p.x = 0;
    p.y = 0;
    result = scalarProduct(p, scalar);
    if(result.x != 0 || result.y != 0) {
        printf("\n\tERROR: Wrong result, expected (0, 0), obtained (%.1f, %.1f)\n", result.x, result.y);
        return false;
    }

    return true;
}

bool test_norm() {
    Point v = {1, 2};
    float result;

    printf("Testing norm...");

    result = norm(v);
    if(!APPROX_EQUAL(result, sqrt(5))) {
        printf("\n\tERROR: Wrong result, expected %.3f, obtained %.3f\n", (float)sqrt(5), result);
        return false;
    }

    v.x = 0;
    v.y = 0;
    result = norm(v);
    if(result != 0) {
        printf("\n\tERROR: Wrong result, expected 0, obtained %.1f\n", result);
        return false;
    }

    return true;
}

bool test_unitVector() {
    Point a = {1, 2};
    Point b = {4, 5};
    Point result;

    printf("Testing unitVector...");

    result = unitVector(a, b);
    if(!APPROX_EQUAL(result.x, 0.707) || !APPROX_EQUAL(result.y, 0.707)) {
        printf("\n\tERROR: Wrong result, expected (0.707, 0.707), obtained (%.3f, %.3f)\n", result.x, result.y);
        return false;
    }

    a.x = 0;
    a.y = 0;
    result = unitVector(a, b);
    if(!APPROX_EQUAL(result.x, 0.625) || !APPROX_EQUAL(result.y, 0.781)) {
        printf("\n\tERROR: Wrong result, expected (0.625, 0.781), obtained (%.3f, %.3f)\n", result.x, result.y);
        return false;
    }

    return true;
}

bool test_distancePoints() {
    Point p1 = {1, 2};
    Point p2 = {4, 5};
    float result;

    printf("Testing distancePoints...");

    result = distancePoints(p1, p2);
    if(!APPROX_EQUAL(result, sqrt(18))) {
        printf("\n\tERROR: Wrong result, expected %.4f, obtained %.4f\n", (float)sqrt(18), result);
        return false;
    }

    p1.x = 0;
    p1.y = 0;
    result = distancePoints(p1, p2);
    if(!APPROX_EQUAL(result, sqrt(41))) {
        printf("\n\tERROR: Wrong result, expected %.4f, obtained %.4f\n", (float)sqrt(41), result);
        return false;
    }

    return true;
}

/* ==================== collision.c tests ==================== */

bool test_intersectionTest() {
    double numerator, denominator;

    printf("Testing intersectionTest...");

    denominator = 1, numerator = -1;
    if (intersectionTest(numerator, denominator) == true) {
        printf("\n\tERROR: Wrong intersection test, with num = %.0f den = %.0f expected false but got true\n", numerator, denominator);
        return false;
    }

    denominator = 1, numerator = 2;
    if (intersectionTest(numerator, denominator) == true) {
        printf("\n\tERROR: Wrong intersection test, with num = %.0f den = %.0f expected false but got true\n", numerator, denominator);
        return false;
    }

    denominator = 1, numerator = 1;
    if (intersectionTest(numerator, denominator) == false) {
        printf("\n\tERROR: Wrong intersection test, with num = %.0f den = %.0f expected true but got false\n", numerator, denominator);
        return false;
    }

    denominator = -1, numerator = 1;
    if (intersectionTest(numerator, denominator) == true) {
        printf("\n\tERROR: Wrong intersection test, with num = %.0f den = %.0f expected false but got true\n", numerator, denominator);
        return false;
    }

    denominator = -1, numerator = -2;
    if (intersectionTest(numerator, denominator) == true) {
        printf("\n\tERROR: Wrong intersection test, with num = %.0f den = %.0f expected false but got true\n", numerator, denominator);
        return false;
    }

    denominator = -2, numerator = -1;
    if (intersectionTest(numerator, denominator) == false) {
        printf("\n\tERROR: Wrong intersection test, with num = %.0f den = %.0f expected true but got false\n", numerator, denominator);
        return false;
    }

    return true;
}


bool test_segmentsIntersect() {
    Segment s1, s2;

    printf("Testing segmentsIntersect...");

    s1 = (Segment){.start = (Point){.x = 0, .y = 1}, .end = (Point){.x = 1, .y = 0}};
    s2 = (Segment){.start = (Point){.x = 0, .y = 0}, .end = (Point){.x = 1, .y = 1}};
    if (segmentsIntersect(s1, s2) == false) {
        printf("\n\tERROR: segments (%.0f, %.0f) - (%.0f, %.0f) and (%.0f, %.0f) - (%.0f, %.0f) should intersect, but got false\n",
        s1.start.x, s1.start.y, s1.end.x, s1.end.y, s2.start.x, s2.start.y, s2.end.x, s2.end.y);
        return false;
    }

    s1 = (Segment){.start = (Point){.x = 1, .y = 0}, .end = (Point){.x = 0, .y = 1}};
    s2 = (Segment){.start = (Point){.x = 0, .y = 0}, .end = (Point){.x = 1, .y = 1}};
    if (segmentsIntersect(s1, s2) == false) {
        printf("\n\tERROR: segments (%.0f, %.0f) - (%.0f, %.0f) and (%.0f, %.0f) - (%.0f, %.0f) should intersect, but got false\n",
        s1.start.x, s1.start.y, s1.end.x, s1.end.y, s2.start.x, s2.start.y, s2.end.x, s2.end.y);
        return false;
    }

    s1 = (Segment){.start = (Point){.x = 0, .y = 1}, .end = (Point){.x = 1, .y = 0}};
    s2 = (Segment){.start = (Point){.x = 0, .y = 0}, .end = (Point){.x = -1, .y = -1}};
    if (segmentsIntersect(s1, s2) == true) {
        printf("\n\tERROR: segments (%.0f, %.0f) - (%.0f, %.0f) and (%.0f, %.0f) - (%.0f, %.0f) should not intersect, but got true\n",
        s1.start.x, s1.start.y, s1.end.x, s1.end.y, s2.start.x, s2.start.y, s2.end.x, s2.end.y);
        return false;
    }

    return true;
}

bool test_squaredDistancePointSegment() {
    Point a = {0, 0};
    Point b = {1, 1};
    Point c = {2, 2};
    float result;

    printf("Testing squaredDistancePointSegment...");

    result = squaredDistancePointSegment(a, b, c);
    if(!APPROX_EQUAL(result, 2)) {
        printf("\n\tERROR: Wrong result, expected 2.0, obtained %.1f\n", result);
        return false;
    }

    c = (Point) {.x = 1, .y = 0};
    result = squaredDistancePointSegment(a, b, c);
    if(!APPROX_EQUAL(result, 0.5)) {
        printf("\n\tERROR: Wrong result, expected 0.50, obtained %.2f\n", result);
        return false;
    }

    return true;
}

bool test_circleCollision() {
    Point p;
    int radius;
    Segment wall = { .start = (Point){0, 0}, .end = (Point){2, 2}};
    bool result;

    printf("Testing circleCollision...");

    p = (Point){0, 2};
    radius = 1;
    result = circleCollision(p, radius, wall);
    if(result == true) {
        printf("\n\tERROR: circle (%.0f, %.0f) with radius %d should not collide with wall (%.0f, %.0f) - (%.0f, %.0f), but got true\n",
        p.x, p.y, radius, wall.start.x, wall.start.y, wall.end.x, wall.end.y);
        return false;
    }

    p = (Point){0, 4};
    radius = 1;
    result = circleCollision(p, radius, wall);
    if(result == true) {
        printf("\n\tERROR: circle (%.0f, %.0f) with radius %d should not collide with wall (%.0f, %.0f) - (%.0f, %.0f), but got true\n",
        p.x, p.y, radius, wall.start.x, wall.start.y, wall.end.x, wall.end.y);
        return false;
    }

    p = (Point){0, 1};
    radius = 1;
    result = circleCollision(p, radius, wall);
    if(result == false) {
        printf("\n\tERROR: circle (%.0f, %.0f) with radius %d should collide with wall (%.0f, %.0f) - (%.0f, %.0f), but got false\n",
        p.x, p.y, radius, wall.start.x, wall.start.y, wall.end.x, wall.end.y);
        return false;
    }

    p = (Point){0, 2};
    radius = 2;
    result = circleCollision(p, radius, wall);
    if(result == false) {
        printf("\n\tERROR: circle (%.0f, %.0f) with radius %d should collide with wall (%.0f, %.0f) - (%.0f, %.0f), but got false\n",
        p.x, p.y, radius, wall.start.x, wall.start.y, wall.end.x, wall.end.y);
        return false;
    }

    return true;
}

bool test_allowedMovement() {
    Level level;
    int radius = 1;
    bool result;

    printf("Testing allowedMovement...");

    level.walls = malloc(2 * sizeof(Segment));
    level.walls[0] = (Segment){.start = (Point){0, 0}, .end = (Point){2, 0}};
    level.walls[1] = (Segment){.start = (Point){2, 0}, .end = (Point){2, 2}};
    level.n_walls = 2;

    Point p_start = {0, 2};
    Point p_end = {4, 0};
    result = allowedMovement(p_start, p_end, radius, level);
    if(result == true) {
        printf("\n\tERROR: movement from (%.0f, %.0f) to (%.0f, %.0f) with radius %d should not be allowed\n",
        p_start.x, p_start.y, p_end.x, p_end.y, radius);
        return false;
    }

    p_start = (Point) {0, 2};
    p_end = (Point) {2, 2.3};
    result = allowedMovement(p_start, p_end, radius, level);
    if(result == true) {
        printf("\n\tERROR: movement from (%.0f, %.0f) to (%.0f, %.0f) with radius %d should not be allowed\n",
        p_start.x, p_start.y, p_end.x, p_end.y, radius);
        return false;
    }

    p_start = (Point) {0, 2};
    p_end = (Point) {0, 3};
    result = allowedMovement(p_start, p_end, radius, level);
    if(result == false) {
        printf("\n\tERROR: movement from (%.0f, %.0f) to (%.0f, %.0f) with radius %d should be allowed\n",
        p_start.x, p_start.y, p_end.x, p_end.y, radius);
        return false;
    }

    p_start = (Point) {0, 2};
    p_end = (Point) {0.5, 0.5};
    result = allowedMovement(p_start, p_end, radius, level);
    if(result == true) {
        printf("\n\tERROR: movement from (%.0f, %.0f) to (%.0f, %.0f) with radius %d should not be allowed\n",
        p_start.x, p_start.y, p_end.x, p_end.y, radius);
        return false;
    }

    free(level.walls);
    return true;
}

/* ==================== movement.c tests ==================== */

bool test_newDestination() {
    Person person;

    printf("Testing newDestination...");

    ini_ran(12345);

    person.position = (Point){.x = 100, .y = 100};
    person.max_step = 10;
    person.angle = 0;
    person.delta_angle = 0;

    bool delta_changed = false;
    float initial_delta = person.delta_angle;
    for(int i = 0; i < 100; i++) {
        newDestination(&person);
        if(!APPROX_EQUAL(person.delta_angle, initial_delta)) {
            delta_changed = true;
            break;
        }
    }
    if(!delta_changed) {
        printf("\n\tERROR: delta_angle never changed after 100 calls\n");
        return false;
    }

    person.angle = 0.5;
    person.delta_angle = 0;
    float old_angle = person.angle;

    Point dest = newDestination(&person);

    float expected_angle = old_angle + person.delta_angle;
    if (expected_angle > M_PI) expected_angle -= 2 * M_PI;
    if (expected_angle < -M_PI) expected_angle += 2 * M_PI;
    if(!APPROX_EQUAL(person.angle, expected_angle)) {
        printf("\n\tERROR: angle = %.4f, expected old_angle + delta_angle = %.4f\n",
               person.angle, expected_angle);
        return false;
    }

    if(person.angle < -M_PI || person.angle > M_PI) {
        printf("\n\tERROR: angle %.4f is outside [-PI, PI]\n", person.angle);
        return false;
    }

    float expected_x = person.position.x + person.step_size * cos(person.angle);
    float expected_y = person.position.y + person.step_size * sin(person.angle);
    if(!APPROX_EQUAL(dest.x, expected_x) || !APPROX_EQUAL(dest.y, expected_y)) {
        printf("\n\tERROR: dest (%.3f, %.3f) != position + step*cos/sin(angle) (%.3f, %.3f)\n",
               dest.x, dest.y, expected_x, expected_y);
        return false;
    }

    if(person.step_size < 0 || person.step_size > person.max_step + EPSILON) {
        printf("\n\tERROR: step_size %.3f outside [0, %.1f]\n", person.step_size, person.max_step);
        return false;
    }

    return true;
}

bool test_updateDestination() {
    Person person;
    Point destination;

    printf("Testing updateDestination...");

    person.position = (Point){.x = 0, .y = 0};
    person.destination = (Point){.x = 0, .y = 0};

    destination = (Point){.x = 5, .y = 5};
    updateDestination(&person, destination);

    if(!APPROX_EQUAL(person.destination.x, 5) || !APPROX_EQUAL(person.destination.y, 5)) {
        printf("\n\tERROR: destination not updated, expected (5, 5), got (%.1f, %.1f)\n",
               person.destination.x, person.destination.y);
        return false;
    }

    if(!APPROX_EQUAL(person.direction.x, 0.707) || !APPROX_EQUAL(person.direction.y, 0.707)) {
        printf("\n\tERROR: direction not updated, expected (0.707, 0.707), got (%.3f, %.3f)\n",
               person.direction.x, person.direction.y);
        return false;
    }

    return true;
}

bool test_movePerson() {
    Person person;
    Level level;
    bool result;

    printf("Testing movePerson...");

    ini_ran(12345);

    person.radius = 1;
    person.velocity = 2;
    person.max_step = 100;
    person.angle = 0;
    person.delta_angle = 0;
    person.log = (Log){.questions_asked = 0, .distance = 0};

    level.walls = malloc(2 * sizeof(Segment));
    level.walls[0] = (Segment){.start = (Point){1.5, 0}, .end = (Point){2, 0}};
    level.walls[1] = (Segment){.start = (Point){2, 0}, .end = (Point){2, 2}};
    level.n_walls = 2;

    person.position = (Point){.x = 0, .y = 0};
    updateDestination(&person, (Point){.x = 0.00001, .y = 0.00001});
    result = movePerson(&person, level, 1);
    if(result == true) {
        printf("\n\tERROR: should return false when destination is too close\n");
        return false;
    }

    person.velocity = 100;
    person.log.distance = 0;
    person.position = (Point){.x = 0, .y = 0};
    updateDestination(&person, (Point){.x = 0, .y = 1});
    result = movePerson(&person, level, 1);
    if(result == false) {
        printf("\n\tERROR: movement to (0, 1) should be allowed\n");
        return false;
    }
    if(!APPROX_EQUAL(person.position.y, 1) && !APPROX_EQUAL(person.log.distance, 1)) {
        printf("\n\tERROR: expected position (0, 1) and distance 1, got (%.3f, %.3f) and %.3f\n",
               person.position.x, person.position.y, person.log.distance);
        return false;
    }

    person.velocity = 2;
    person.log.distance = 0;
    person.position = (Point){.x = 0, .y = 0};
    updateDestination(&person, (Point){.x = 0, .y = 1});
    result = movePerson(&person, level, 0.1);
    if(result == false) {
        printf("\n\tERROR: partial movement to (0, 1) should be allowed\n");
        return false;
    }
    if(!APPROX_EQUAL(person.position.x, 0) || !APPROX_EQUAL(person.position.y, 0.2)) {
        printf("\n\tERROR: expected position (0, 0.2), got (%.3f, %.3f)\n",
               person.position.x, person.position.y);
        return false;
    }
    if(!APPROX_EQUAL(person.log.distance, 0.2)) {
        printf("\n\tERROR: expected distance 0.2, got %.3f\n", person.log.distance);
        return false;
    }

    person.velocity = 10;
    person.log.distance = 0;
    person.position = (Point){.x = 1, .y = 1};
    updateDestination(&person, (Point){.x = 3, .y = 1});
    result = movePerson(&person, level, 1);
    if(result == true) {
        printf("\n\tERROR: movement through wall should not be allowed\n");
        return false;
    }
    if(!APPROX_EQUAL(person.position.x, 1) || !APPROX_EQUAL(person.position.y, 1)) {
        printf("\n\tERROR: position should not change on blocked movement\n");
        return false;
    }

    free(level.walls);
    return true;
}

/* ==================== random.c tests ==================== */

bool test_vonMises() {
    float result;
    int i;

    printf("Testing vonMises...");

    ini_ran(42);

    for(i = 0; i < 100; i++) {
        result = vonMises(0, 0, 2);
        if(result < -M_PI || result > M_PI) {
            printf("\n\tERROR: vonMises returned %.4f which is outside [-pi, pi]\n", result);
            return false;
        }
    }

    ini_ran(42);
    float sum = 0;
    int n = 1000;
    float angle = 0;
    for(i = 0; i < n; i++) {
        angle = vonMises(angle, 0, 10);
        sum += fabs(angle);
    }
    float mean_abs = sum / n;
    if(mean_abs > M_PI / 2) {
        printf("\n\tERROR: with high kappa, angles should cluster near mu=0, but mean |angle| = %.3f\n", mean_abs);
        return false;
    }

    ini_ran(42);
    sum = 0;
    angle = 0;
    float kappa0_mean;
    for(i = 0; i < n; i++) {
        angle = vonMises(angle, 0, 0);
        sum += fabs(angle);
    }
    kappa0_mean = sum / n;
    if(kappa0_mean < mean_abs) {
        printf("\n\tERROR: kappa=0 should spread more than kappa=10, but got %.3f < %.3f\n",
               kappa0_mean, mean_abs);
        return false;
    }

    ini_ran(42);
    angle = 0;
    for(i = 0; i < 100; i++) {
        float old_angle = angle;
        angle = vonMises(angle, 0, 2);
        float diff = angle - old_angle;
        // Account for wrapping around +-PI
        if(diff > M_PI) diff -= 2 * M_PI;
        if(diff < -M_PI) diff += 2 * M_PI;
        if(diff < -M_PI / 4 - EPSILON || diff > M_PI / 4 + EPSILON) {
            printf("\n\tERROR: angle change %.4f is outside [-pi/4, pi/4] (old=%.4f, new=%.4f)\n",
                   diff, old_angle, angle);
            return false;
        }
    }

    return true;
}

/* ==================== person.c tests ==================== */

bool test_updatePanic() {
    Person student;
    float dt = 1.0;

    printf("Testing updatePanic...");

    student.status = SEARCHING;
    student.panic = (Panic){
        .current_stress = 50,
        .maximum_stress = 100,
        .stress_recovery_rate = 10,
        .stress_searching_rate = 5,
        .stress_chasing_rate = 2,
        .levels = STRESS_LEVELS
    };
    updatePanic(&student, dt);
    if(!APPROX_EQUAL(student.panic.current_stress, 55)) {
        printf("\n\tERROR: expected stress 55 after searching, got %.1f\n", student.panic.current_stress);
        return false;
    }

    student.status = CHASING;
    student.panic.current_stress = 50;
    updatePanic(&student, dt);
    if(!APPROX_EQUAL(student.panic.current_stress, 52)) {
        printf("\n\tERROR: expected stress 52 after chasing, got %.1f\n", student.panic.current_stress);
        return false;
    }

    student.status = ASKING;
    student.panic.current_stress = 50;
    updatePanic(&student, dt);
    if(!APPROX_EQUAL(student.panic.current_stress, 40)) {
        printf("\n\tERROR: expected stress 40 after asking, got %.1f\n", student.panic.current_stress);
        return false;
    }

    student.status = ASKING;
    student.panic.current_stress = 5;
    updatePanic(&student, dt);
    if(!APPROX_EQUAL(student.panic.current_stress, 0)) {
        printf("\n\tERROR: stress should clamp to 0, got %.1f\n", student.panic.current_stress);
        return false;
    }

    student.status = SEARCHING;
    student.panic.current_stress = 98;
    student.panic.stress_searching_rate = 50;
    updatePanic(&student, dt);
    if(!APPROX_EQUAL(student.panic.current_stress, 100)) {
        printf("\n\tERROR: stress should clamp to 100, got %.1f\n", student.panic.current_stress);
        return false;
    }

    student.panic.current_stress = 60;
    student.status = ASKING;
    student.panic.stress_recovery_rate = 0;
    updatePanic(&student, dt);
    int expected_levels = (int)floor(60.0 / 100.0 * STRESS_LEVELS);
    if(student.panic.levels != expected_levels) {
        printf("\n\tERROR: expected %d levels at 60%% stress, got %d\n", expected_levels, student.panic.levels);
        return false;
    }

    return true;
}

bool test_findProfessor() {
    Person student;
    Level level;

    printf("Testing findProfessor...");

    student.position = (Point){.x = 400, .y = 400};
    student.angle = 0;
    student.detection_shape = CIRCLE;
    updateDetectionArea(&student);

    level.n_professors = 3;
    level.professors = malloc(3 * sizeof(Person));
    level.professors[0].position = (Point){.x = 500, .y = 400}; // 100 away, in range
    level.professors[1].position = (Point){.x = 800, .y = 400}; // 400 away, out of range
    level.professors[2].position = (Point){.x = 450, .y = 400}; // 50 away, in range, closest

    int result = findProfessor(student, level);
    if(result != 2) {
        printf("\n\tERROR: expected closest professor index 2, got %d\n", result);
        free(level.professors);
        return false;
    }

    level.professors[0].position = (Point){.x = 800, .y = 800};
    level.professors[2].position = (Point){.x = 900, .y = 900};
    updateDetectionArea(&student);

    result = findProfessor(student, level);
    if(result != -1) {
        printf("\n\tERROR: expected -1 (no professor in range), got %d\n", result);
        free(level.professors);
        return false;
    }

    level.n_professors = 0;
    result = findProfessor(student, level);
    if(result != -1) {
        printf("\n\tERROR: expected -1 with no professors, got %d\n", result);
        free(level.professors);
        return false;
    }

    free(level.professors);
    return true;
}

bool test_removeProfessor() {
    Level level;

    printf("Testing removeProfessor...");

    level.n_professors = 3;
    level.professors = malloc(3 * sizeof(Person));
    level.professors[0].position = (Point){.x = 10, .y = 10};
    level.professors[1].position = (Point){.x = 20, .y = 20};
    level.professors[2].position = (Point){.x = 30, .y = 30};

    removeProfessor(&level, 1);
    if(level.n_professors != 2) {
        printf("\n\tERROR: expected 2 professors after removal, got %d\n", level.n_professors);
        free(level.professors);
        return false;
    }

    if(!APPROX_EQUAL(level.professors[1].position.x, 30) ||
       !APPROX_EQUAL(level.professors[1].position.y, 30)) {
        printf("\n\tERROR: expected professor[1] at (30, 30), got (%.1f, %.1f)\n",
               level.professors[1].position.x, level.professors[1].position.y);
        free(level.professors);
        return false;
    }

    if(!APPROX_EQUAL(level.professors[0].position.x, 10) ||
       !APPROX_EQUAL(level.professors[0].position.y, 10)) {
        printf("\n\tERROR: expected professor[0] at (10, 10), got (%.1f, %.1f)\n",
               level.professors[0].position.x, level.professors[0].position.y);
        free(level.professors);
        return false;
    }

    removeProfessor(&level, 0);
    if(level.n_professors != 1) {
        printf("\n\tERROR: expected 1 professor after second removal, got %d\n", level.n_professors);
        free(level.professors);
        return false;
    }

    if(!APPROX_EQUAL(level.professors[0].position.x, 30) ||
       !APPROX_EQUAL(level.professors[0].position.y, 30)) {
        printf("\n\tERROR: expected professor[0] at (30, 30), got (%.1f, %.1f)\n",
               level.professors[0].position.x, level.professors[0].position.y);
        free(level.professors);
        return false;
    }

    free(level.professors);
    return true;
}

/* ==================== main ==================== */

int main() {
    int passed = 0, n_test = 0;
    bool status = true;

    printf("**************************************************\n*              TESTING STUDENT CODE              *\n**************************************************\n\n");
    printf(" - geometry.c\n");
    RUN_TEST(test_substractVectors);
    RUN_TEST(test_dotProduct);
    RUN_TEST(test_scalarProduct);
    RUN_TEST(test_norm);
    RUN_TEST(test_unitVector);
    RUN_TEST(test_distancePoints);
    printf(" - collision.c\n");
    RUN_TEST(test_intersectionTest);
    RUN_TEST(test_segmentsIntersect);
    RUN_TEST(test_squaredDistancePointSegment);
    RUN_TEST(test_circleCollision);
    RUN_TEST(test_allowedMovement);
    printf(" - random.c\n");
    RUN_TEST(test_vonMises);
    printf(" - movement.c\n");
    RUN_TEST(test_newDestination);
    RUN_TEST(test_updateDestination);
    RUN_TEST(test_movePerson);
    printf(" - person.c\n");
    RUN_TEST(test_updatePanic);
    RUN_TEST(test_removeProfessor);
    RUN_TEST(test_findProfessor);

    printf("\nTests successfully completed: %d/%d\n", passed, n_test);

    return n_test - passed;
}