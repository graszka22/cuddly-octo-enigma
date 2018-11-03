#include "core.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

void get_vector_from_points(point_t* vector, point_t p0, point_t p1) {
    vector->x = p1.x-p0.x;
    vector->y = p1.y-p0.y;
}

double distance(point_t p0, point_t p1) {
    point_t vec;
    get_vector_from_points(&vec, p0, p1);
    double x = vec.x;
    double y = vec.y;
    return sqrt(x*x+y*y);
}

void normalize_vector(point_t* vector) {
    double x = vector->x;
    double y = vector->y;
    double d = sqrt(x*x+y*y);
    vector->x = x/d;
    vector->y = y/d;
}

void get_tangents(int num_of_points, point_t* points, point_t* tan1, point_t* tan2) {
    get_vector_from_points(tan1, points[0], points[1]);
    normalize_vector(tan1);
    get_vector_from_points(tan2, points[num_of_points-1], points[num_of_points-2]);
    normalize_vector(tan2);
}

void get_chord_lengths(int num_of_points, point_t* points, double* lengths) {
    double total_length = 0;
    for(int i = 1; i < num_of_points; ++i) {
        total_length += distance(points[i-1], points[i]);
    }
    double current_length = 0;
    lengths[0] = 0;
    for(int i = 1; i < num_of_points; ++i) {
        current_length += distance(points[i-1], points[i]);
        lengths[i] = current_length/total_length;
    }
}

cubic_bezier_t fit_bezier(int num_of_points, point_t* points) {
    point_t tan1, tan2;
    get_tangents(num_of_points, points, &tan1, &tan2);
    
    double* chord_lengths = malloc(num_of_points*sizeof(double));
    get_chord_lengths(num_of_points, points, chord_lengths);


    free(chord_lengths);
}

double lerp(double t, double x1, double x2) {
    return x1+t*(x2-x1);
}

void get_bezier_points(int num_of_points, cubic_bezier_t bezier, point_t* points) {
    assert(num_of_points > 1);
    for(int i = 0; i < num_of_points; ++i) {
        double t = ((double)(i))/(num_of_points-1);
        point_t p0 = {
            lerp(t, bezier.p0.x, bezier.p1.x),
            lerp(t, bezier.p0.y, bezier.p1.y)
        };
        point_t p1 = {
            lerp(t, bezier.p1.x, bezier.p2.x),
            lerp(t, bezier.p1.y, bezier.p2.y)
        };
        point_t p2 = {
            lerp(t, bezier.p2.x, bezier.p3.x),
            lerp(t, bezier.p2.y, bezier.p3.y)
        };
        point_t p3 = {
            lerp(t, p0.x, p1.x),
            lerp(t, p0.y, p1.y)
        };
        point_t p4 = {
            lerp(t, p1.x, p2.x),
            lerp(t, p1.y, p2.y)
        };
        points[i].x = lerp(t, p3.x, p4.x);
        points[i].y = lerp(t, p3.y, p4.y);
    }
}
