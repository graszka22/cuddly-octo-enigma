#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "draw.h"
#include "core.h"

int main(int argc, char const *argv[])
{
    int n;
    scanf("%d", &n);
    point_t* points = malloc(n*sizeof(point_t));
    for(int i = 0; i < n; ++i) {
        scanf("%lf %lf", &points[i].x, &points[i].y);
    }
    cubic_bezier_t bezier = fit_bezier(n, points);
    image_t image = create_image(300, 300);
    draw_bezier(image, bezier);
    draw_points(image, n, points);
    save_to_png(image, "obrazek.png");
    destroy_image(image);
    free(points);
    return 0;
}
