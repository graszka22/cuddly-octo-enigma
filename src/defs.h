#ifndef DEFS_H
#define DEFS_H

#include <cairo.h>
#include <stdint.h>

typedef struct {
    double x, y;
} point_t;

typedef struct {
    point_t p0, p1, p2, p3;
} cubic_bezier_t;

typedef struct {
    cairo_surface_t *surface;
    cairo_t *cr;
} image_t;

typedef struct {
    uint8_t* data;
    int width, height, stride;
    cairo_format_t format;
} image_data_t;

#endif