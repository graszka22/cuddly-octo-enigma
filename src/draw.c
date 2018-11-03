#include "draw.h"
#include <math.h>
#include "assert.h"

image_t create_image(int width, int height) {
    image_t image;
    image.surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    image.cr = cairo_create(image.surface);
    cairo_set_source_rgb (image.cr, 1, 1, 1);
    cairo_paint(image.cr);
    return image;
}

void destroy_image(image_t image) {
    cairo_destroy(image.cr);
    cairo_surface_destroy(image.surface);
}

void save_to_png(image_t image, const char* filename) {
    assert(cairo_surface_write_to_png(image.surface, filename) == CAIRO_STATUS_SUCCESS);
}

void draw_bezier(image_t image, cubic_bezier_t bezier) {
    cairo_set_source_rgba(image.cr, 0, 0, 0, 1);
    cairo_set_line_width(image.cr, 5);
    cairo_move_to(image.cr, bezier.p0.x, bezier.p0.y);
    cairo_curve_to(image.cr,
                   bezier.p1.x, bezier.p1.y,
                   bezier.p2.x, bezier.p2.y,
                   bezier.p3.x, bezier.p3.y);
    cairo_stroke(image.cr);
}

void draw_points(image_t image, int num_of_points, point_t* points) {
    cairo_set_source_rgba(image.cr, 1, 0.2, 0.2, 1);
    for(int i = 0; i < num_of_points; ++i) {
        cairo_arc(image.cr, points[i].x, points[i].y, 3.0, 0, 2*M_PI);
        cairo_fill(image.cr);
    }
}