#ifndef DRAW_H
#define DRAW_H

#include <cairo.h>
#include <stdint.h>
#include "defs.h"

image_t create_image(int width, int height);
void destroy_image(image_t image);
void save_to_png(image_t image, const char* filename);
image_data_t load_data_from_png(const char* filename);
void save_data_to_png(const char* filename, image_data_t image_data);
void destroy_image_data(image_data_t image_data);
void save_grayscale(uint8_t* grayscale, uint8_t* junctions, int width, int height);

void draw_bezier(image_t image, cubic_bezier_t bezier);
void draw_points(image_t image, int num_of_points, point_t* points);

#endif