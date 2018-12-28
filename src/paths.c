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

uint8_t* detect_junctions(uint8_t* image, int width, int height) {
    uint8_t* junctions = malloc(width*height);
    memset(junctions, 0, width*height);
    int8_t masks[12][3][3] = {
        {{0, 1, 0}, {-1, 1, -1}, {1, -1, 1}},
        {{1, -1, 0}, {-1, 1, -1}, {1, -1, 1}},
        {{0, -1, 1}, {1, 1, -1}, {0, 1, 0}},
        {{1, -1, 0}, {-1, 1, 1}, {0, 1, 0}},
        {{0, -1, 1}, {-1, 1, -1}, {1, -1, 1}},
        {{0, -1, 1}, {1, 1, -1}, {0, -1, 1}},
        {{0, 1, 0}, {-1, 1, 1}, {1, -1, 0}},
        {{1, -1, 1}, {-1, 1, -1}, {0, -1, 1}},
        {{1, -1, 1}, {-1, 1, -1}, {0, 1, 0}},
        {{1, -1, 0}, {-1, 1, 1}, {1, -1, 0}},
        {{1, -1, 1}, {-1, 1, -1}, {1, -1, 0}},
        {{0, 1, 0}, {1, 1, -1}, {0, -1, 1}}
    };
    for(int y = 1; y < height-1; ++y)
    for(int x = 1; x < width-1; ++x) {
        for(int i = 0; i < 12; ++i) {
            int sum = 0;
            for(int yy = 0; yy < 3; ++yy)
            for(int xx = 0; xx < 3; ++xx)
                sum += masks[i][yy][xx]*image[(y+yy-1)*width+x+xx-1];
            if(sum == 4) {
                junctions[y*width+x] = 1;
                break;
            }
        }
    }
    return junctions;
}

path_t* get_paths(uint8_t* image, int width, int height, int* number_of_paths) {
    make_8_connect(image, width, height);
    uint8_t* junctions = detect_junctions(image, width, height);
    save_grayscale(image, junctions, width, height);
    path_t* paths = malloc(1);
    *number_of_paths = 0;
    return paths;
}