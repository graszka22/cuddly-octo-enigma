#include <stdlib.h>
#include "core.h"
#include "paths.h"
#include "pcc.h"

cubic_bezier_t* vectorize_image(image_data_t image_data) {
    uint8_t* lines = identify_lines(image_data);
    int number_of_paths;
    path_t* paths = get_paths(lines, image_data.width, image_data.height, &number_of_paths);
    free(lines);
    for(int i = 0; i < number_of_paths; ++i)
        destroy_path(paths[i]);
    free(paths);
}