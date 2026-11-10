#pragma once

#include "bn.h"

typedef struct {
    f32 min, max;
} Interval;

static inline Interval intervalEmpty(void) {
    return (Interval){ INFINITY, -INFINITY };
}

static inline Interval intervalUniverse(void) {
    return (Interval){ -INFINITY, INFINITY };
}

static inline Interval intervalInit(f32 min, f32 max) {
    return (Interval){ min, max };
}

static inline f32 intervalSize(Interval i) {
    return i.max - i.min;
}

static inline bool intervalContains(Interval i, f32 x) {
    return i.min <= x && x <= i.max;
}

static inline bool intervalSurrounds(Interval i, f32 x) {
    return i.min < x && x < i.max;
}

