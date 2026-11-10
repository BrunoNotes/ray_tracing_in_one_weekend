#pragma once

#include "bn.h"

const f32 infinity = INFINITY;
const f32 pi = 3.1415926535897932385;

inline f32 f32DegreesToRadians(f32 degrees) {
    return degrees * pi / 180.0;
}
