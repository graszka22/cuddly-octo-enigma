#ifndef CORE_H
#define CORE_H

#include "defs.h"

cubic_bezier_t fit_bezier(int num_of_points, point_t* points);
void get_bezier_points(int num_of_points, cubic_bezier_t bezier, point_t* points);

#endif