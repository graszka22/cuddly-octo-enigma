#include "core.h"
#include <assert.h>

cubic_bezier_t fit_bezier(int num_of_points, point_t* points) {
    cubic_bezier_t bezier = {
        points[0],
        points[1],
        points[2],
        points[3]
    };
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
