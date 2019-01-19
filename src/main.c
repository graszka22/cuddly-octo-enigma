#include <stdio.h>
#include <stdlib.h>
#include <argparse.h>

#include "defs.h"
#include "draw.h"
#include "core.h"
#include "curve_fitting.h"

static const char *const usage[] = {
    "curve input_image [options]",
    NULL,
};


int main(int argc, char const *argv[])
{
    const char* input_path = NULL;
    const char* output_path = NULL;
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_STRING('o', "output", &output_path, "output image path"),
        OPT_END(),
    };
    struct argparse argparse;
    argparse_init(&argparse, options, usage, 0);
    argparse_describe(&argparse, "\nconverts png linearts into svg!", NULL);
    argc = argparse_parse(&argparse, argc, argv);
    if(argc != 1) {
        argparse_usage(&argparse);
        return 1;
    }
    input_path = *argv;

    image_data_t image_data = load_data_from_png(input_path);
    vectorize_image(image_data);
    destroy_image_data(image_data);
    return 0;
}
