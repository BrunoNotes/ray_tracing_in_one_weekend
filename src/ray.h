#pragma once

#include "bn.h"
#include "interval.h"
#include "utils.h"

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
