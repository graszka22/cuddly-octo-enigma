#include "utils.h"
#include <math.h>
#include <stdlib.h>

double lerp(double t, double x1, double x2) {
    return x1+t*(x2-x1);
}

point_t vec_diff(point_t p0, point_t p1) {
    point_t vec = {
        p1.x-p0.x,
        p1.y-p0.y
    };
    return vec;
}

point_t vec_add(point_t p0, point_t p1) {
    point_t vec = {
        p1.x+p0.x,
        p1.y+p0.y
    };
    return vec;
}

point_t vec_scale(point_t v, double s) {
    point_t vec = {
        v.x*s,
        v.y*s
    };
    return vec;
}

double vec_dot(point_t p0, point_t p1) {
    return p0.x*p1.x+p0.y*p1.y;
}

double squared_distance(point_t p0, point_t p1) {
    point_t vec = vec_diff(p0, p1);
    double x = vec.x;
    double y = vec.y;
    return x*x+y*y;
}

double distance(point_t p0, point_t p1) {
    return sqrt(squared_distance(p0, p1));
}

point_t normalize_vector(point_t vector) {
    double x = vector.x;
    double y = vector.y;
    double d = sqrt(x*x+y*y);
    point_t ret = {
        x/d, y/d
    };
    return ret;
}