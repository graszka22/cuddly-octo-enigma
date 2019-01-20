#include "options.h"
#include <argparse.h>
#include <stdlib.h>

options_t program_options = {
    .debug = false,
    .kernels_count = 5,
    .newton_iterations = 10,
    .median_filter = 2,
    .threshold = 0,
    .min_dot = 1,
    .kernels_exponent = 1.2,
    .epsilon = 2,
    .psi = 5,
    .output_path = "result.svg",
    .line_width = 0,
};

static const char *const usage[] = {
    "curve input_image [options]",
    NULL,
};

void parse_options(int argc, char const *argv[]) {
    struct argparse_option options[] = {
        OPT_HELP(),

        OPT_GROUP("Basic options"),
        OPT_STRING('o', "output", &(program_options.output_path), "output image path"),
        OPT_BOOLEAN('d', "debug", &(program_options.debug), "generate debug images"),

        OPT_GROUP("Line detection"),
        OPT_FLOAT('m', "min_dot", &(program_options.min_dot), "size in piksels of the smallest kernel = size of the least wide line detected"),
        OPT_INTEGER('k', "kernels_count", &(program_options.kernels_count), "how many kernels do we generate"),
        OPT_FLOAT('c', "kernels_exponent", &(program_options.kernels_exponent), "the subsequent kernels will be c times larger"),
        OPT_FLOAT('s', "median_filter", &(program_options.median_filter), "median filter size"),
        OPT_FLOAT('t', "threshold", &(program_options.threshold), "kernels merger threshold"),

        OPT_GROUP("Curve fitting"),
        OPT_INTEGER('n', "newton_iterations", &(program_options.newton_iterations), "how many times we iterate in the Newton method algorithm"),
        OPT_FLOAT('e', "epsilon", &(program_options.epsilon), "if the error is smaller than that, we conclude the line is properly fitted"),
        OPT_FLOAT('p', "psi", &(program_options.psi), "if the error is bigger than that, we split the points and fit more curves"),

        OPT_GROUP("Line drawing"),
        OPT_FLOAT('w', "line_width", &(program_options.line_width), "the width of the drawn lines"),
        OPT_END(),
    };
    struct argparse argparse;
    argparse_init(&argparse, options, usage, 0);
    argparse_describe(&argparse, "\nconverts png linearts into svg!", NULL);
    argc = argparse_parse(&argparse, argc, argv);
    if(argc != 1) {
        argparse_usage(&argparse);
        exit(1);
    }
    program_options.input_path = *argv;
}