#include "pcc.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "draw.h"
#include "options.h"

const float PI = 3.1415;

typedef struct {
    int dot_size;
    float* data;
} kdot_t;

void destroy_kdot(kdot_t dot) {
    free(dot.data);
}

float gauss(float x, float sigma) {
    return exp(-(x/sigma)*(x/sigma)/2);
}

int next_odd(float f) {
    int floor_v = floor(f);
    return floor_v + (!(floor_v%2));
}

float* get_grayscale(image_data_t image_data) {
    float* grayscale = malloc(image_data.width*image_data.height*sizeof(float));
    for(int y = 0; y < image_data.height; ++y)
    for(int x = 0; x < image_data.width; ++x) {
        uint32_t pixel = ((uint32_t*)image_data.data)[y*image_data.stride/4+x];
        uint8_t r = (pixel >> 16)&0xFF;
        uint8_t g = (pixel >> 8)&0xFF;
        uint8_t b = (pixel)&0xFF;
        float value = 0.3*r + 0.59*g + 0.11*b;
        grayscale[y*image_data.width+x] = 1-value/255;
    }
    return grayscale;
}

kdot_t* generate_dots() {
    const int dots_count = program_options.kernels_count;
    float C = program_options.kernels_exponent;
    kdot_t* dots = malloc(dots_count*sizeof(kdot_t));
    float sigma = program_options.min_dot/3;
    for(int i = 0; i < dots_count; ++i) {
        const int dot_size = next_odd(7*sigma);
        float* data = malloc(dot_size*dot_size*sizeof(float));
        for(int y = 0; y < dot_size; ++y)
        for(int x = 0; x < dot_size; ++x) {
            data[y*dot_size+x] = gauss(x-dot_size/2, sigma)*gauss(y-dot_size/2, sigma);
        }
        kdot_t dot = {
            dot_size,
            data
        };
        dots[i] = dot;
        sigma *= C;
    }
    return dots;
}

float** generate_pcc_images(image_data_t image_data, float* image) {
    kdot_t* kdots = generate_dots();
    const int dots_count = program_options.kernels_count;
    for(int i = 0; i < dots_count; ++i) {
        char buf[20];
        sprintf(buf, "debug/dot%d.png", i);
        debug_grayscale(kdots[i].data, kdots[i].dot_size, kdots[i].dot_size, buf);
    }
    float** images = malloc(dots_count*sizeof(float*));
    for(int i = 0; i < dots_count; ++i) {
        
        const int width = image_data.width;
        const int height = image_data.height;
        images[i] = malloc(width*height*sizeof(float));
        float* T = kdots[i].data;
        
        float uT = 0;
        const int dw = kdots[i].dot_size;
        for(int j = 0; j < dw*dw; ++j)
            uT += T[j];
        uT /= dw*dw;

        for(int y = dw/2; y < height-dw/2; ++y)
        for(int x = dw/2; x < width-dw/2; ++x) {
            float uI = 0;
            float sum1 = 0, sum2 = 0, sum3 = 0;
            for(int j = 0; j < dw; ++j)
            for(int k = 0; k < dw; ++k)
                uI += image[(y+j-dw/2)*width+(x+k-dw/2)];
            uI /= dw*dw;
            for(int j = 0; j < dw; ++j)
            for(int k = 0; k < dw; ++k) {
                const float v = image[(y+j-dw/2)*width+(x+k-dw/2)] - uI;
                const float w = T[j*dw+k] - uT;
                sum1 += v*w;
                sum2 += v*v;
                sum3 += w*w;
            }
            images[i][y*width+x] = (fabs(sum2*sum3) < 0.0001f) ? -1 : sum1/sqrt(sum2*sum3);
        }
        destroy_kdot(kdots[i]);
    }
    free(kdots);
    return images;
}

uint8_t* merge_pcc_images(float** PCCdots, int width, int height) {
    const int dots_count = program_options.kernels_count;
    uint8_t* merged_images = malloc(sizeof(uint8_t)*width*height);
    for(int y = 0; y < height; ++y)
    for(int x = 0; x < width; ++x) {
        float min = PCCdots[0][y*width+x], max = PCCdots[0][y*width+x];
        for(int i = 1; i < dots_count; ++i) {
            min = fminf(min, PCCdots[i][y*width+x]);
            max = fmaxf(max, PCCdots[i][y*width+x]);
        }
        const float v = fabs(max) > fabs(min) ? max : min;
        merged_images[y*width+x] = v > program_options.threshold ? 1 : 0;
    }
    return merged_images;
}

void median_filter(uint8_t* image, int width, int height) {
    const int ws = program_options.median_filter;
    for(int y = ws/2; y < height-ws/2; ++y)
    for(int x = ws/2; x < width-ws/2; ++x) {
        int ones = 0, zeroes = 0;
        for(int i = 0; i < ws; ++i)
        for(int j = 0; j < ws; ++j) {
            if(image[(y+i)*width+x+j]) ++ones; else ++zeroes;
        }
        image[y*width+x] = ones > zeroes;
    }
}

void thinning(uint8_t* image, int width, int height) {
    int changes = 0;
    uint8_t* copy = malloc(width*height);
    while(1) {
        memcpy(copy, image, width*height);
        changes = 0;
        for(int y = 1; y < height-1; ++y)
        for(int x = 1; x < width-1; ++x) {
            if(!image[y*width+x]) continue;
            uint8_t n[9] = {
                copy[(y)*width+x],
                copy[(y-1)*width+x],
                copy[(y-1)*width+x+1],
                copy[(y)*width+x+1],
                copy[(y+1)*width+x+1],
                copy[(y+1)*width+x],
                copy[(y+1)*width+x-1],
                copy[(y)*width+x-1],
                copy[(y-1)*width+x-1]
            };
            int A = 0;
            int B = 0;
            for(int i = 1; i < 9; ++i) B += n[i];
            for(int i = 1; i < 8; ++i) A += (n[i] == 0 && n[i+1] == 1);
            A += (n[1] == 1 && n[8] == 0);
            if(B >= 2 && B <= 6 && A == 1 && n[1]*n[3]*n[5] == 0 && n[3]*n[5]*n[7] == 0) {
                image[y*width+x] = 0;
                ++changes;
            }
        }
        if(changes == 0) break;
        memcpy(copy, image, width*height);
        changes = 0;
        for(int y = 1; y < height-1; ++y)
        for(int x = 1; x < width-1; ++x) {
            if(!image[y*width+x]) continue;
            uint8_t n[9] = {
                copy[(y)*width+x],
                copy[(y-1)*width+x],
                copy[(y-1)*width+x+1],
                copy[(y)*width+x+1],
                copy[(y+1)*width+x+1],
                copy[(y+1)*width+x],
                copy[(y+1)*width+x-1],
                copy[(y)*width+x-1],
                copy[(y-1)*width+x-1]
            };
            int A = 0;
            int B = 0;
            for(int i = 1; i < 9; ++i) B += n[i];
            for(int i = 1; i < 8; ++i) A += (!n[i] && n[i+1]);
            A += (n[1] == 1 && n[8] == 0);
            if(B >= 2 && B <= 6 && A == 1 && n[1]*n[3]*n[7] == 0 && n[1]*n[5]*n[7] == 0) {
                image[y*width+x] = 0;
                ++changes;
            }
        }
        if(changes == 0) break;
    }
    free(copy);
}

uint8_t* threshold_lines(image_data_t image_data, float* image) {
    uint8_t* res = malloc(sizeof(uint8_t)*image_data.width*image_data.height);
    for(int y = 0; y < image_data.height; ++y)
    for(int x = 0; x < image_data.width; ++x) {
        res[y*image_data.width+x] = image[y*image_data.width+x] > 0.2;
    }
    return res;
}

float* get_maxi_pcc(float** PCCdots, int width, int height) {
    const int dots_count = program_options.kernels_count;
    kdot_t* kdots = generate_dots();
    float* maxi_pcc = malloc(sizeof(float)*width*height);
    for(int y = 0; y < height; ++y)
    for(int x = 0; x < width; ++x) {
        float max_dot = 1;
        float max_pcc = 0;
        for(int i = 0; i < dots_count; ++i) {
            float pcc = PCCdots[i][y*width+x];
            if(pcc > max_pcc)
                max_pcc = pcc, max_dot = kdots[i].dot_size;
        }
        maxi_pcc[y*width+x] = max_dot;
    }
    for(int i = 0; i < dots_count; ++i)
        destroy_kdot(kdots[i]);
    free(kdots);
    return maxi_pcc;
}

uint8_t* identify_lines(image_data_t image_data, float** maxi_pcc) {
    const int dots_count = program_options.kernels_count;
    float* grayscale = get_grayscale(image_data);
    debug_grayscale(grayscale, image_data.width, image_data.height, "debug/grayscale.png");
    float** PCCdots = generate_pcc_images(image_data, grayscale);
    for(int i = 0; i < dots_count; ++i) {
        char buf[20];
        sprintf(buf, "debug/pcc%d.png", i);
        debug_pcc(PCCdots[i], image_data.width, image_data.height, buf);
    }
    uint8_t* merged_image = merge_pcc_images(PCCdots, image_data.width, image_data.height);
    *maxi_pcc = get_maxi_pcc(PCCdots, image_data.width, image_data.height);
    debug_binary(merged_image, image_data.width, image_data.height, "debug/merged_pcc.png");
    for(int i = 0; i < dots_count; ++i)
        free(PCCdots[i]);
    free(PCCdots);
    free(grayscale);
    median_filter(merged_image, image_data.width, image_data.height);
    debug_binary(merged_image, image_data.width, image_data.height, "debug/median_filter.png");
    thinning(merged_image, image_data.width, image_data.height);
    debug_binary(merged_image, image_data.width, image_data.height, "debug/thinned_image.png");
    return merged_image;
}
