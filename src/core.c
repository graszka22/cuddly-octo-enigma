#include "core.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

point_t vec_diff(point_t p0, point_t p1) {
    point_t vec = {
        p1.x-p0.x,
        p1.y-p0.y
    };
    return vec;
}

point_t vec_add(point_t p0, point_t p1) {
    point_t vec = {
        p1.x+p0.x,
        p1.y+p0.y
    };
    return vec;
}

point_t vec_scale(point_t v, double s) {
    point_t vec = {
        v.x*s,
        v.y*s
    };
    return vec;
}

double vec_dot(point_t p0, point_t p1) {
    return p0.x*p1.x+p0.y*p1.y;
}

double distance(point_t p0, point_t p1) {
    point_t vec = vec_diff(p0, p1);
    double x = vec.x;
    double y = vec.y;
    return sqrt(x*x+y*y);
}

point_t normalize_vector(point_t vector) {
    double x = vector.x;
    double y = vector.y;
    double d = sqrt(x*x+y*y);
    point_t ret = {
        x/d, y/d
    };
    return ret;
}

void get_tangents(int num_of_points, point_t* points, point_t* tan1, point_t* tan2) {
    point_t tmp = vec_diff(points[0], points[1]);
    tmp = normalize_vector(tmp);
    tan1->x = tmp.x;
    tan1->y = tmp.y;
    tmp = vec_diff(points[num_of_points-1], points[num_of_points-2]);
    tmp = normalize_vector(tmp);
    tan2->x = tmp.x;
    tan2->y = tmp.y;
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

cubic_bezier_t fit_single_bezier(int num_of_points, point_t* points, double* chord_lengths,
                                 point_t tan1, point_t tan2) {
    double* b_poly = malloc(4*num_of_points*sizeof(double));
    point_t* A1 = malloc(num_of_points*sizeof(point_t));
    point_t* A2 = malloc(num_of_points*sizeof(point_t));
    for(int i = 0; i < num_of_points; ++i) {
        double u = chord_lengths[i];
        double u2 = u*u;
        double u3 = u2*u;
        double u_inv = 1-u;
        double u_inv2 = u_inv*u_inv;
        double u_inv3 = u_inv2*u_inv;
        b_poly[4*i] = u_inv3;
        b_poly[4*i+1] = 3*u*u_inv2;
        b_poly[4*i+2] = 3*u2*u_inv;
        b_poly[4*i+3] = u3;
        A1[i] = vec_scale(tan1, b_poly[4*i+1]);
        A2[i] = vec_scale(tan2, b_poly[4*i+2]);
    }
    
    double X[2] = {0, 0};
    double C[2][2] = {{0, 0}, {0, 0}};
    point_t V0 = points[0];
    point_t V3 = points[num_of_points-1];

    for(int i = 0; i < num_of_points; ++i) {
        point_t p = vec_add( 
            vec_add(
                vec_scale(V0, b_poly[4*i]),
                vec_scale(V0, b_poly[4*i+1])
            ),
            vec_add(
                vec_scale(V3, b_poly[4*i+2]),
                vec_scale(V3, b_poly[4*i+3])
            )
        );
        p = vec_diff(p, points[i]);
        X[0] += vec_dot(p, A1[i]);
        X[1] += vec_dot(p, A2[i]);
    }
    for(int i = 0; i < num_of_points; ++i) {
        C[0][0] += vec_dot(A1[i], A1[i]);
        C[0][1] += vec_dot(A1[i], A2[i]);
        C[1][0] += vec_dot(A1[i], A2[i]);
        C[1][1] += vec_dot(A2[i], A2[i]);
    }
    double alpha1 = (X[0]*C[1][1]-X[1]*C[0][1])/(C[0][0]*C[1][1]-C[0][1]*C[1][0]);
    double alpha2 = (C[0][0]*X[1]-X[0]*C[1][0])/(C[0][0]*C[1][1]-C[0][1]*C[1][0]);
    free(b_poly);
    free(A1);
    free(A2);
    cubic_bezier_t bezier = {
        V0,
        vec_add(vec_scale(tan1, alpha1), V0),
        vec_add(vec_scale(tan2, alpha2), V3),
        V3
    };
    return bezier;
}

cubic_bezier_t fit_bezier(int num_of_points, point_t* points) {
    point_t tan1, tan2;
    get_tangents(num_of_points, points, &tan1, &tan2);
    
    double* chord_lengths = malloc(num_of_points*sizeof(double));
    get_chord_lengths(num_of_points, points, chord_lengths);

    cubic_bezier_t bezier = fit_single_bezier(num_of_points, points, chord_lengths, tan1, tan2);

    free(chord_lengths);
    return bezier;
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
