#pragma once

#include "bn.h"

typedef Vec3f32 Point3;
typedef Vec3f32 Color;

const f32 infinity = INFINITY;
const f32 pi = 3.1415926535897932385;

inline f32 f32DegreesToRadians(f32 degrees) {
    return degrees * pi / 180.0;
}

static void writeColor(FILE* f_ptr, Color* pixel_color) {
    f32 r = pixel_color->r;
    f32 g = pixel_color->g;
    f32 b = pixel_color->b;

    i32 rb = (i32)(255.999 * r);
    i32 gb = (i32)(255.999 * g);
    i32 bb = (i32)(255.999 * b);

    fprintf(f_ptr, "%d %d %d\n", rb, gb, bb);
}
