#include "core.h"

cubic_bezier_t* vectorize_image(image_data_t image_data) {
    uint8_t* lines = identify_lines(image_data);
    free(lines);
}