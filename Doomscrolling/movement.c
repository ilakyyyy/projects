#include "head.h"

/**
 * Returns a new destination for the person. The destination is a random point 
 * within the maximum step distance of the person from its current position.
 */
Point newDestination(Person *person) {
    float new_delta_theta, new_theta;
    new_delta_theta = vonMises(person->delta_angle, VONMISES_MU, VONMISES_KAPPA);
    new_theta = person->angle + new_delta_theta;
    if (new_theta > M_PI) {
        new_theta -= 2*M_PI;
    }
    if (new_theta < -M_PI) {
        new_theta += 2*M_PI;
    }
    person->angle = new_theta;
    person->delta_angle = new_delta_theta;

    person->step_size = uniformRandomIn(0, person->max_step);
    float new_x, new_y;
    new_x = person->position.x+person->step_size*cos(new_theta);
    new_y = person->position.y+person->step_size*sin(new_theta);
    Point new_pos = { .x = new_x, .y = new_y};
    return(new_pos);
}

/**
 * Updates the destination of the person and sets its direction to the unit vector
 * from the current position to the new destination.
 */
void updateDestination(Person *person, Point destination) {
    person->destination = destination;
    person->direction = unitVector(person->position, destination);
}

/**
 * Moves the person towards its destination in a timestep dt as long as the movement is allowed. 
 * If the distance to the destination is less than 0.0001f,
 * it doesn't move.
 */
bool movePerson(Person *person, Level level, float dt) {
    float distance = distancePoints(person->position, person->destination);
    if (distance < (float)1e-4) {
        // updateDestination(person, newDestination(person));
        return(false);
    }

    float step = person->velocity*dt;
    if (step >= distance) {
        if (allowedMovement(person->position, person->destination, person->radius, level)) {
            person->position = person->destination;
            person->log.distance += distance;

            updateDestination(person, newDestination(person));
            return(true);
        }
    }
    else {
        Point new_destination = {.x = person->position.x + person->direction.x*step, .y = person->position.y + person->direction.y*step};
        if (allowedMovement(person->position, new_destination, person->radius, level)) {
            person->position = new_destination;
            person->log.distance += step;

            // updateDestination(person, newDestination(person));
            return(true);
        }
    }
    return false;
}