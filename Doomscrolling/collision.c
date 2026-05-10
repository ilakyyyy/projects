#include "head.h"

/**
 * Performs the test proposed by Franklin Antonio, returning false
 * if the lines do not intersect and true otherwise.
*/
bool intersectionTest(double numerator, double denominator) {
    if (denominator > 0) {
        if (numerator < 0 || numerator > denominator) {
            return(false);
        }
    }
    else if (numerator > 0 || numerator < denominator) {
        return(false);
    }
    return(true);
}

/**
 * Returns true if segments s1 and s2 intersect, false otherwise.
*/
bool segmentsIntersect(Segment s1, Segment s2) {
    float alpha_num, beta_num, denominator;
    Point a, b, c;
    a = substractVectors(s1.end, s1.start);
    b = substractVectors(s2.start, s2.end);
    c = substractVectors(s1.start, s2.start);

    denominator = a.y*b.x-a.x*b.y;
    alpha_num = b.y*c.x-b.x*c.y;
    if (intersectionTest(alpha_num, denominator) == false) {
        return(false);
    }
    beta_num = a.x*c.y-a.y*c.x;
    return(intersectionTest(beta_num, denominator));
}

/**
 * Returns the squared distance between point c and the segment ab.
 * The function returns the squared distance to avoid computing the square root.
 */
float squaredDistancePointSegment(Point a, Point b, Point c) {
    Point ab = substractVectors(b,a), ac = substractVectors(c,a), bc = substractVectors(c,b);
    float e = dotProduct(ac, ab);
    if (e <= 0.0f) {
        return(dotProduct(ac, ac));
    }
    float f = dotProduct(ab, ab);
    if (e >= f) {
        return(dotProduct(bc, bc));
    }

    //If it falls inside ab:
    return(dotProduct(ac, ac)- e*e/f);
}

/**
 * Returns true if a circle defined by the point p_end and its radius, collides
 * with a wall. The function uses squared distance to avoid computing the square root.
 */
bool circleCollision(Point p_end, int radius, Segment wall) {
    float sqdistance;
    sqdistance = squaredDistancePointSegment(wall.start, wall.end, p_end);
    if (sqdistance > ((float)radius*radius)) {
        return(false);
    }
    else {
        return(true);
    }
}

/**
 * Checks if the trajectory from p_start to p_end intersects with any wall. It also checks if 
 * a circle located in p_end with a given radius collides with any wall. Returns true if the
 * movement is allowed, false otherwise.
 */
bool allowedMovement(Point p_start, Point p_end, int radius, Level level) {
    Segment s1 = {.start = p_start, .end = p_end};
    for(int i=0; i<level.n_walls; i++) {
        if (segmentsIntersect(s1, level.walls[i]) == true) {
            return(false);
        }
        if (circleCollision(p_end, radius, level.walls[i]) == true) {
            return(false);
        }
    }
    return(true);
}