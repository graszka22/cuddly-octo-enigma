#ifndef PATHS_H
#define PATHS_H
#include <stdint.h>
#include "defs.h"

typedef struct {
    int points_count;
    point_t* points;
} path_t;

void destroy_path(path_t path);
path_t* get_paths(uint8_t* image, int width, int height, int* number_of_paths);

#endif