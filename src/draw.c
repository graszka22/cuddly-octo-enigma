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

image_data_t load_data_from_png(const char* filename) {
    cairo_surface_t* surface = cairo_image_surface_create_from_png(filename);
    int height = cairo_image_surface_get_height(surface);
    int width = cairo_image_surface_get_width(surface);
    cairo_format_t format = cairo_image_surface_get_format(surface);
    int stride = cairo_format_stride_for_width(format, width);
    uint8_t* data = cairo_image_surface_get_data(surface);
    uint8_t* data_copy = malloc(height*stride);
    memcpy(data_copy, data, height*stride);

    image_data_t ret = {
        .data = data_copy,
        .width = width,
        .height = height,
        .stride = stride,
        .format = format
    };
    cairo_surface_destroy(surface);
    return ret;
}

void save_data_to_png(const char* filename, image_data_t image_data) {
    cairo_surface_t* surface = cairo_image_surface_create_for_data(
        image_data.data,
        image_data.format,
        image_data.width,
        image_data.height,
        image_data.stride
    );
    assert(cairo_surface_write_to_png(surface, filename) == CAIRO_STATUS_SUCCESS);
    cairo_surface_destroy(surface);
}

void destroy_image_data(image_data_t image_data) {
    free(image_data.data);
}

void draw_bezier(image_t image, cubic_bezier_t bezier) {
    cairo_set_source_rgba(image.cr, 0, 0, 0, 1);
    cairo_set_line_width(image.cr, 3);
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

void save_grayscale(uint8_t* grayscale, uint8_t* junctions, int width, int height) {
    uint32_t* data = malloc(width*height*sizeof(uint32_t));
    for(int y = 0; y < height; ++y)
    for(int x = 0; x < width; ++x) {
        uint32_t val = grayscale[y*width+x]*255;
        if(junctions[y*width+x])
            data[y*width+x] = 0xFF0000;
        else
            data[y*width+x] = (val << 16) | (val << 8) | val;
    }
    cairo_surface_t* surface = cairo_image_surface_create_for_data(
        data,
        CAIRO_FORMAT_RGB24,
        width,
        height,
        width*4
    );
    assert(cairo_surface_write_to_png(surface, "grayscale.png") == CAIRO_STATUS_SUCCESS);
    cairo_surface_destroy(surface);
    free(data);
}