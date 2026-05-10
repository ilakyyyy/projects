#include "head.h"

/**
 * Returns the vector p1 - p2.
 */
Point substractVectors(Point p1, Point p2) {
    Point result = { .x = p1.x-p2.x, .y = p1.y-p2.y };
    return (result);
}

/**
 * Returns the dot product of two vectors.
 */
float dotProduct(Point p1, Point p2) {
    float result = p1.x*p2.x+p1.y*p2.y;
    return (result);
}


/**
 * Returns the scalar product of a vector and a scalar.
 */
Point scalarProduct(Point p, float scalar) {
    Point result = { .x = scalar*p.x, .y = scalar*p.y }; 
    return (result);
}


/**
 * Returns the norm of a vector.
 */
float norm(Point p) {
    float result = sqrt(pow(p.x,2)+pow(p.y,2));
    return (result);
}


/**
 * Returns the unit vector of the vector b - a.
 */
Point unitVector(Point a, Point b) {
    Point u = substractVectors(b, a);
    float mod = norm(u);
    return (scalarProduct(u, 1/mod));
}

/**
 * Returns the distance between two points.
 */
float distancePoints(Point a, Point b) {
    Point subtraction = substractVectors(a,b);
    float distance = norm(subtraction);
    return (distance);
}