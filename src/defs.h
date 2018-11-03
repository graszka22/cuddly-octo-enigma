#ifndef DEFS_H
#define DEFS_H

typedef struct {
    double x, y;
} point_t;

typedef struct {
    point_t p0, p1, p2, p3;
} cubic_bezier_t;

#endif