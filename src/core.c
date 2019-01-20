#include <stdlib.h>
#include <stdio.h>
#include "core.h"
#include "paths.h"
#include "pcc.h"
#include "curve_fitting.h"
#include "draw.h"

cubic_bezier_t* vectorize_image(image_data_t image_data) {
    uint8_t* lines = identify_lines(image_data);
    int number_of_paths;
    path_t* paths = get_paths(lines, image_data.width, image_data.height, &number_of_paths);
    FILE* output_file = fopen ("result.svg", "wb");
    
    image_t image = create_svg_image(image_data.width, image_data.height, output_file);

    for(int i = 0; i < number_of_paths; ++i) {
        path_t path = paths[i];
        int num_of_beziers;
        cubic_bezier_t* beziers = fit_bezier(path.points_count, path.points, &num_of_beziers);
    
        for(int i = 0; i < num_of_beziers; ++i) {
            draw_bezier(image, beziers[i]);
        }

        free(beziers);
    }
    cairo_surface_flush(image.surface);
    cairo_surface_finish(image.surface);
    destroy_image(image);
    fclose (output_file);

    free(lines);
    for(int i = 0; i < number_of_paths; ++i)
        destroy_path(paths[i]);
    free(paths);
}