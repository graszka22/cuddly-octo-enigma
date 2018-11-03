#ifndef DRAW_H
#define DRAW_H

#include <cairo.h>
#include "defs.h"

typedef struct {
    cairo_surface_t *surface;
    cairo_t *cr;
} image_t;

image_t create_image(int width, int height);
void destroy_image(image_t image);
void save_to_png(image_t image, const char* filename);

void draw_bezier(image_t image, cubic_bezier_t bezier);
void draw_points(image_t image, int num_of_points, point_t* points);

#endif