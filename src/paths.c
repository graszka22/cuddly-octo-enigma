#include "paths.h"
#include <stdlib.h>
#include "draw.h"

void destroy_path(path_t path) {
    free(path.points);
}

void apply_mask(uint8_t* image, uint8_t* output, int8_t masks[][3][3], int masks_count,
                int width, int height, uint8_t output_val, int valid_sum) {
    for(int y = 1; y < height-1; ++y)
    for(int x = 1; x < width-1; ++x) {
        for(int i = 0; i < masks_count; ++i) {
            int sum = 0;
            for(int yy = 0; yy < 3; ++yy)
            for(int xx = 0; xx < 3; ++xx)
                sum += masks[i][yy][xx]*image[(y+yy-1)*width+x+xx-1];
            if(sum == valid_sum) {
                output[y*width+x] = output_val;
                break;
            }
        }
    }
}

void make_8_connect(uint8_t* image, int width, int height) {
    int8_t masks[4][3][3] = {
        {{0, 1, 0}, {0, 1, 1}, {-1, 0, 0}},
        {{0, 1, 0}, {1, 1, 0}, {0, 0, -1}},
        {{0, 0, -1}, {1, 1, 0}, {0, 1, 0}},
        {{-1, 0, 0}, {0, 1, 1}, {0, 1, 0}}
    };
    apply_mask(image, image, masks, 4, width, height, 0, 3);
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
    apply_mask(image, junctions, masks, 12, width, height, 1, 4);
    return junctions;
}

uint8_t* detect_endpoints(uint8_t* image, int width, int height) {
    uint8_t* endpoints = malloc(width*height);
    memset(endpoints, 0, width*height);
    int8_t masks[8][3][3] = {
        {{0, 1, 0}, {-1, 1, -1}, {-1, -1, -1}},
        {{1, -1, -1}, {-1, 1, -1}, {-1, -1, -1}},
        {{-1, -1, -1}, {-1, 1, -1}, {1, -1, -1}},
        {{0, -1, -1}, {1, 1, -1}, {0, -1, -1}},
        {{-1, -1, -1}, {-1, 1, -1}, {-1, -1, 1}},
        {{-1, -1, -1}, {-1, 1, -1}, {0, 1, 0}},
        {{-1, -1, 0}, {-1, 1, 1}, {-1, -1, 0}},
        {{-1, -1, 1}, {-1, 1, -1}, {-1, -1, -1}}
    };
    apply_mask(image, endpoints, masks, 8, width, height, 1, 2);
    return endpoints;
}

path_t* get_paths(uint8_t* image, int width, int height, int* number_of_paths) {
    make_8_connect(image, width, height);
    uint8_t* junctions = detect_junctions(image, width, height);
    uint8_t* endpoints = detect_endpoints(image, width, height);
    save_grayscale(image, endpoints, width, height);
    path_t* paths = malloc(1);
    *number_of_paths = 0;
    free(junctions);
    free(endpoints);
    return paths;
}