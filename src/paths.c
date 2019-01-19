#include "paths.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
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

void detect_endpoints(uint8_t* image, uint8_t* junctions, int width, int height) {
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
    apply_mask(image, junctions, masks, 8, width, height, 1, 2);
}

path_t* detect_paths(uint8_t* image, uint8_t* junctions, int width, int height, int* number_of_paths) {
    point_t* current_path = malloc(width*height*sizeof(point_t));
    path_t* paths = malloc(width*height*sizeof(path_t));
    int k = 0, n = 0;
    for(int y = 0; y < height; ++y)
    for(int x = 0; x < width; ++x) {
        if(!junctions[y*width+x]) continue;
        while(1) {
            k = 0;
            point_t c = {x, y};
            current_path[k++] = c;
            int xx = x, yy = y;
            while(1) {
                bool end_of_path = true;
                for(int i = -1; i < 2; ++i)
                for(int j = -1; j < 2; ++j) {
                    if(i == 0 && j == 0) continue;
                    if(xx+j < 0 || xx+j >= width || yy+i < 0 || yy+i >= height) continue;
                    if(image[(yy+i)*width+xx+j]) {
                        end_of_path = false;
                        c.x = xx+j;
                        c.y = yy+i;
                        current_path[k++] = c;
                        if(junctions[(yy+i)*width+xx+j])
                            end_of_path = true;
                        image[(yy+i)*width+xx+j] = 0;
                        xx += j;
                        yy += i;
                        goto BREAK_LOOP;
                    }
                }
                BREAK_LOOP:
                if(end_of_path)
                    break;
            }
            if(k == 1) break;
            if(k > 5) {
                point_t* new_path = malloc(k*sizeof(point_t));
                memcpy(new_path, current_path, sizeof(point_t)*k);
                path_t path = {
                    .points_count = k,
                    .points = new_path
                };
                paths[n++] = path;
            }
        }
    }

    for(int y = 0; y < height; ++y)
    for(int x = 0; x < width; ++x) {
        if(!image[y*width+x]) continue;
        k = 0;
        point_t c = {x, y};
        current_path[k++] = c;
        int xx = x, yy = y;
        while(1) {
            bool end_of_path = true;
            for(int i = -1; i < 2; ++i)
            for(int j = -1; j < 2; ++j) {
                if(i == 0 && j == 0) continue;
                if(xx+j < 0 || xx+j >= width || yy+i < 0 || yy+i >= height) continue;
                if(image[(yy+i)*width+xx+j]) {
                    end_of_path = false;
                    c.x = xx+j;
                    c.y = yy+i;
                    current_path[k++] = c;
                    image[(yy+i)*width+xx+j] = 0;
                    xx += j;
                    yy += i;
                    goto BREAK_LOOP2;
                }
            }
            BREAK_LOOP2:
            if(end_of_path)
                break;
        }
        if(k > 5) {
            point_t* new_path = malloc(k*sizeof(point_t));
            memcpy(new_path, current_path, sizeof(point_t)*k);
            path_t path = {
                .points_count = k,
                .points = new_path
            };
            paths[n++] = path;
        }
    }
    free(current_path);
    *number_of_paths = n;
    return paths;
}

path_t* get_paths(uint8_t* image, int width, int height, int* number_of_paths) {
    make_8_connect(image, width, height);
    debug_binary(image, width, height, "debug/8connect.png");
    uint8_t* junctions = detect_junctions(image, width, height);
    debug_grayscale_with_points(image, junctions, width, height, "debug/junctions.png");
    detect_endpoints(image, junctions, width, height);
    debug_grayscale_with_points(image, junctions, width, height, "debug/endpoints.png");
    path_t* paths = detect_paths(image, junctions, width, height, number_of_paths);
    free(junctions);
    return paths;
}