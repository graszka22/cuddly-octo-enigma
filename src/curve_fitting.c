#include "curve_fitting.h"
#include "utils.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

const int max_newton_iterations = 10;
const double eps = 2;
const double psi = 10;

point_t evaluate(cubic_bezier_t curve, double t) {
    double inv = (1-t);
    double b_poly[] = {
        inv*inv*inv,
        3*t*inv*inv,
        3*t*t*inv,
        t*t*t,
    };
    point_t V[] = {
        curve.p0, curve.p1, curve.p2, curve.p3
    };
    point_t ret = {0, 0};
    for(int i = 0; i < 4; ++i) {
        ret.x += b_poly[i]*V[i].x;
        ret.y += b_poly[i]*V[i].y;
    }
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

void get_error(int num_of_points, point_t* points, double* chord_lengths, cubic_bezier_t curve,
               double* error, int* max_error_index) {
    double max_dist = 0;
    int index = 0;
    for(int i = 0; i < num_of_points; ++i) {
        point_t q = evaluate(curve, chord_lengths[i]);
        double dist = squared_distance(q, points[i]);
        if(dist > max_dist) {
            index = i;
            max_dist = dist;
        }
    }
    *error = max_dist;
    *max_error_index = index;
}

void recalculate_chord_lengths(int num_of_points, point_t* points, double* chord_lengths,
                               cubic_bezier_t curve) {
    point_t V[] = {
        curve.p0, curve.p1, curve.p2, curve.p3
    };
    point_t V1[] = {
        vec_diff(V[0], V[1]),
        vec_diff(V[1], V[2]),
        vec_diff(V[2], V[3])
    };
    point_t V2[] = {
        vec_diff(V1[0], V1[1]),
        vec_diff(V1[1], V1[2])
    };

    for(int i = 0; i < num_of_points; ++i) {
        double t = chord_lengths[i];
        double t2 = t*t;
        double t3 = t2*t;
        double it = 1-t;
        double it2 = it*it;
        double it3 = it2*it;
        point_t Q0 = {
            V[0].x*it3 + 3*V[1].x*t*it2 + 3*V[2].x*t2*it + V[3].x*t3,
            V[0].y*it3 + 3*V[1].y*t*it2 + 3*V[2].y*t2*it + V[3].y*t3,
        };
        point_t Q1 = {
            3*(V1[0].x*it2 + 2*V1[1].x*t*it + V1[2].x*t2),
            3*(V1[0].y*it2 + 2*V1[1].y*t*it + V1[2].y*t2),
        };
        point_t Q2 = {
            6*(V2[0].x*it + V2[1].x*t),
            6*(V2[0].y*it + V2[1].y*t),
        };
        point_t P = points[i];
        double nominator = (Q0.x - P.x)*Q1.x + (Q0.y - P.y)*Q1.y;
        double denominator = Q1.x*Q1.x + Q1.y*Q1.y + (Q0.x - P.x)*Q2.x + (Q0.y - P.y)*Q2.y;
        if(denominator == 0.0f) continue;
        chord_lengths[i] -= nominator / denominator;
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
        point_t p = {
            V0.x*b_poly[4*i] + V0.x*b_poly[4*i+1] + V3.x*b_poly[4*i+2] + V3.x*b_poly[4*i+3],
            V0.y*b_poly[4*i] + V0.y*b_poly[4*i+1] + V3.y*b_poly[4*i+2] + V3.y*b_poly[4*i+3],
        };
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
    double det = C[0][0]*C[1][1]-C[0][1]*C[1][0];
    double alpha1 = det == 0 ? 0 : (X[0]*C[1][1]-X[1]*C[0][1])/det;
    double alpha2 = det == 0 ? 0 : (C[0][0]*X[1]-X[0]*C[1][0])/det;
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

cubic_bezier_t* fit_bezier(int num_of_points, point_t* points, int* num_of_beziers) {
    point_t tan1, tan2;
    get_tangents(num_of_points, points, &tan1, &tan2);
    
    double* chord_lengths = malloc(num_of_points*sizeof(double));
    get_chord_lengths(num_of_points, points, chord_lengths);

    cubic_bezier_t bezier = fit_single_bezier(num_of_points, points, chord_lengths, tan1, tan2);

    for(int i = 0; i < max_newton_iterations; ++i) {
        double error;
        int index;
        get_error(num_of_points, points, chord_lengths, bezier, &error, &index);
        if(error < eps) break;
        if(error < psi) {
            recalculate_chord_lengths(num_of_points, points, chord_lengths, bezier);
            bezier = fit_single_bezier(num_of_points, points, chord_lengths, tan1, tan2);
        } else {
            int num_of_beziers_left;
            int num_of_beziers_right;
            cubic_bezier_t* beziers_left = fit_bezier(index+1, points, &num_of_beziers_left);
            cubic_bezier_t* beziers_right = fit_bezier(num_of_points-index, points+index, &num_of_beziers_right);
            cubic_bezier_t* ret = malloc((num_of_beziers_left+num_of_beziers_right)*sizeof(cubic_bezier_t));
            memcpy(ret, beziers_left, num_of_beziers_left*sizeof(cubic_bezier_t));
            memcpy(ret+num_of_beziers_left, beziers_right, num_of_beziers_right*sizeof(cubic_bezier_t));
            free(beziers_left);
            free(beziers_right);
            free(chord_lengths);
            *num_of_beziers = num_of_beziers_left+num_of_beziers_right;
            return ret;
        }
    }

    free(chord_lengths);
    *num_of_beziers = 1;
    cubic_bezier_t* ret = malloc(sizeof(cubic_bezier_t));
    ret[0] = bezier;
    return ret;
}