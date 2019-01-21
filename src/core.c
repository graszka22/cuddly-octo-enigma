#include <stdlib.h>
#include <stdio.h>
#include "defs.h"
#include "core.h"
#include "paths.h"
#include "pcc.h"
#include "curve_fitting.h"
#include "draw.h"
#include "options.h"

void vectorize_image() {
    image_data_t image_data = load_data_from_png(program_options.input_path);
    float* maxi_pcc;
    uint8_t* lines = identify_lines(image_data, &maxi_pcc);
    int number_of_paths;
    path_t* paths = get_paths(lines, image_data.width, image_data.height, &number_of_paths);
    FILE* output_file = fopen (program_options.output_path, "wb");
    
    image_t image = create_svg_image(image_data.width, image_data.height, output_file);

    for(int i = 0; i < number_of_paths; ++i) {
        path_t path = paths[i];
        int num_of_beziers;
        cubic_bezier_t* beziers = fit_bezier(path.points_count, path.points, &num_of_beziers);
        float line_width = 0;
        if(program_options.line_width == 0) {
            for(int j = 0; j < path.points_count; ++j) {
                line_width += maxi_pcc[(int)(path.points[j].y)*image_data.width+(int)path.points[j].x];
            }
            line_width /= path.points_count;
        } else {
            line_width = program_options.line_width;
        }
        
        for(int i = 0; i < num_of_beziers; ++i) {
            draw_bezier(image, beziers[i], line_width);
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
    free(maxi_pcc);
    destroy_image_data(image_data);
}