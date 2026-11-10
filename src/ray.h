#pragma once

#include "bn.h"
#include "interval.h"
#include "utils.h"

typedef Vec3f32 Point3;
typedef Vec3f32 Color;

typedef struct {
    Point3 origin;
    Vec3f32 direction;
} Ray;

Point3 rayAt(Ray* ray, f32 t) {
    return vec3f32Add(ray->origin, vec3f32MultScalar(ray->direction, t));
}

typedef enum {
    HitRecordType_Sphere,
} HitRecordType;

typedef struct {
    Point3 p;
    Vec3f32 normal;
    f32 t;
    bool front_face;
    HitRecordType type;

    Point3 center; // for sphere
    f32 radius;    // for sphere
} HitRecord;

bn_arrayPrototype(HitRecord);

void hitRecordSetFaceNormal(
    HitRecord* hr, const Ray* r, const Vec3f32* outward_normal
) {
    // Sets the hit record normal vector.
    // NOTE: the parameter `outward_normal` is assumed to have unit length.

    hr->front_face = vec3f32Dot(r->direction, *outward_normal) < 0;
    hr->normal =
        hr->front_face ? *outward_normal : vec3f32Negative(*outward_normal);
}

bool hitRecordHit(Ray* r, Interval ray_t, HitRecord* rec) {
    switch (rec->type) {
    case HitRecordType_Sphere: {
        Vec3f32 oc = vec3f32Sub(rec->center, r->origin);
        f32 a = vec3f32LengthSquared(r->direction);
        f32 h = vec3f32Dot(r->direction, oc);
        f32 c = vec3f32LengthSquared(oc) - rec->radius * rec->radius;

        f32 discrimant = h * h - a * c;

        if (discrimant < 0)
            return false;

        f32 sqrtd = sqrt(discrimant);

        // Find the nearest root that lies in the acceptable range.
        f32 root = (h - sqrtd) / a;
        if (!intervalSurrounds(ray_t, root)) {
            root = (h + sqrtd) / a;
            if (!intervalSurrounds(ray_t, root))
                return false;
        }

        rec->t = root;
        rec->p = rayAt(r, rec->t);
        rec->normal =
            vec3f32DivScalar(vec3f32Sub(rec->p, rec->center), rec->radius);

        Vec3f32 outward_normal =
            vec3f32DivScalar(vec3f32Sub(rec->p, rec->center), rec->radius);

        hitRecordSetFaceNormal(rec, r, &outward_normal);

        return true;
    } break;
    default:
        return false;
    }
}

bool hitRecordHitArray(
    Ray* r,
    Interval ray_t,
    BN_arrayHitRecord* objects,
    HitRecord* rec
) {
    HitRecord temp_rec;
    bool hit_anything = false;
    f32 closest_so_far = ray_t.max;

    for (u32 i = 0; i < objects->length; i++) {
        temp_rec = objects->items[i];
        if (hitRecordHit(r, intervalInit(ray_t.min, closest_so_far), &temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            *rec = temp_rec;
        }
    }

    return hit_anything;
}

Color rayColor(Ray* r, BN_arrayHitRecord* world) {
    HitRecord rec;
    if (hitRecordHitArray(r, intervalInit(0, infinity), world, &rec)) {
        return vec3f32MultScalar(
            vec3f32Add(rec.normal, (Color){.r = 1.0, .g = 1.0, .b = 1.0}), 0.5
        );
    }

    Vec3f32 unit_direction = vec3f32UnitVector(r->direction);
    f32 a = 0.5 * (unit_direction.y + 1.0);

    // “linear blend”, or “linear interpolation”, commonly referred to as a lerp
    // between two values.
    // blended_value = (1-a) * start_value + a * end_value
    return vec3f32Add(
        vec3f32MultScalar((Color){.x = 1.0, .y = 1.0, .z = 1.0}, (1.0 - a)),
        vec3f32MultScalar((Color){.x = 0.5, .y = 0.7, .z = 1.0}, a)
    );
}
