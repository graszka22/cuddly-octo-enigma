#include "paths.h"
#include <stdlib.h>
#include "draw.h"

void destroy_path(path_t path) {
    free(path.points);
}

void make_8_connect(uint8_t* image, int width, int height) {
    int8_t masks[4][3][3] = {
        {{0, 1, 0}, {0, 1, 1}, {-1, 0, 0}},
        {{0, 1, 0}, {1, 1, 0}, {0, 0, -1}},
        {{0, 0, -1}, {1, 1, 0}, {0, 1, 0}},
        {{-1, 0, 0}, {0, 1, 1}, {0, 1, 0}}
    };
    for(int y = 1; y < height-1; ++y)
    for(int x = 1; x < width-1; ++x) {
        for(int i = 0; i < 4; ++i) {
            int sum = 0;
            for(int yy = 0; yy < 3; ++yy)
            for(int xx = 0; xx < 3; ++xx)
                sum += masks[i][yy][xx]*image[(y+yy-1)*width+x+xx-1];
            if(sum == 3) {
                image[y*width+x] = 0;
                break;
            }
        }
    }
}

path_t* get_paths(uint8_t* image, int width, int height, int* number_of_paths) {
    make_8_connect(image, width, height);
    save_grayscale(image, width, height);
    path_t* paths = malloc(1);
    *number_of_paths = 0;
    return paths;
}