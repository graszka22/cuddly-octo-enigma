#ifndef UTILS_H
#define UTILS_H

#include "defs.h"

double lerp(double t, double x1, double x2);
point_t vec_diff(point_t p0, point_t p1);
point_t vec_add(point_t p0, point_t p1);
point_t vec_scale(point_t v, double s);
double vec_dot(point_t p0, point_t p1);
double squared_distance(point_t p0, point_t p1);
double distance(point_t p0, point_t p1);
point_t normalize_vector(point_t vector);

#endif