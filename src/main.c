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
    /*
    int n;
    scanf("%d", &n);
    point_t* points = malloc(n*sizeof(point_t));
    for(int i = 0; i < n; ++i) {
        scanf("%lf %lf", &points[i].x, &points[i].y);
    }
    int num_of_beziers;
    cubic_bezier_t* beziers = fit_bezier(n, points, &num_of_beziers);
    image_t image = create_image(300, 300);
    for(int i = 0; i < num_of_beziers; ++i) {
        draw_bezier(image, beziers[i]);
    }
    draw_points(image, n, points);
    save_to_png(image, "obrazek.png");
    destroy_image(image);
    free(beziers);
    free(points);
    */
    
   /*
   image_t image = create_image(300, 300);
    cubic_bezier_t beziers[] = {
        {
            {55, 199},
            {145, 187},
            {19, 17},
            {129, 56}
        },
        {
            {129, 56},
            {240, 91},
            {229, 143},
            {176, 140}
        },
        {
            {176, 140},
            {122, 136},
            {140, 243},
            {219, 272}
        }
    };
    for(int i = 0; i < 3; ++i) {
        draw_bezier(image, beziers[i]);
        point_t points[20];
        get_bezier_points(20, beziers[i], points);
        for(int j = 0; j < 20; ++j) {
            printf("%lf %lf\n", points[j], points[j].y);
        }
    }
    save_to_png(image, "obrazek.png");
    destroy_image(image);
    */
    return 0;
}
