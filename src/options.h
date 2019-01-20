#ifndef OPTIONS_H
#define OPTIONS_H
#include <stdbool.h>

typedef struct {
    bool debug;
    int kernels_count, newton_iterations;
    float min_dot, kernels_exponent, median_filter, threshold,
        epsilon, psi, line_width;
    const char* output_path;
    const char* input_path;
} options_t;

extern options_t program_options;

void parse_options(int argc, char const *argv[]);

#endif