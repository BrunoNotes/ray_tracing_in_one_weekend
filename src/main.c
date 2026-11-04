#define BN_IMPLEMENTATION
#include "bn.h"

/*
   https://raytracing.github.io/books/RayTracingInOneWeekend.html

   stoped at: 6.7. Common Constants and Utility Functions
*/

const f32 infinity = INFINITY;
const f32 pi = 3.1415926535897932385;

inline f32 f32DegreesToRadians(f32 degrees) {
    return degrees * pi / 180.0;
}

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

bool hitRecordHit(Ray* r, f32 ray_tmin, f32 ray_tmax, HitRecord* rec) {
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
        if (root <= ray_tmin || ray_tmax <= root) {
            root = (h + sqrtd) / a;
            if (root <= ray_tmin || ray_tmax <= root)
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
    f32 ray_tmin,
    f32 ray_tmax,
    BN_arrayHitRecord* objects,
    HitRecord* rec
) {
    HitRecord temp_rec;
    bool hit_anything = false;
    f32 closest_so_far = ray_tmax;

    for (u32 i = 0; i < objects->length; i++) {
        temp_rec = objects->items[i];
        if (hitRecordHit(r, ray_tmin, ray_tmax, &temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            *rec = temp_rec;
        }
    }

    return hit_anything;
}

Color rayColor(Ray* r, BN_arrayHitRecord* world) {
    HitRecord rec;
    if (hitRecordHitArray(r, 0, infinity, world, &rec)) {
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

void writeColor(FILE* f_ptr, Color* pixel_color) {
    f32 r = pixel_color->r;
    f32 g = pixel_color->g;
    f32 b = pixel_color->b;

    i32 rb = (i32)(255.999 * r);
    i32 gb = (i32)(255.999 * g);
    i32 bb = (i32)(255.999 * b);

    fprintf(f_ptr, "%d %d %d\n", rb, gb, bb);
}

int main(void) {
    bn_initCxt(BN_DEFAULT_CONTEXT_PARAMS);

    // image
    f32 aspect_ratio = 16.0 / 9.0;
    i32 img_width = 400;

    // Calculate the image height, and ensure that it's at least 1.
    i32 img_height = (i32)(img_width / aspect_ratio);
    img_height = (img_height < 1) ? 1 : img_height;

    // World
    BN_arrayHitRecord world = bn_daInit();

    HitRecord sphere = (HitRecord){
        .center = (Point3){.x = 0.0, .y = -100.5, .z = -1.0},
        .radius = 100.0,
        .type = HitRecordType_Sphere
    };
    bn_daAppend(&world, sphere, &bn_ctx->allocator);

    sphere = (HitRecord){
        .center = (Point3){.x = 0.0, .y = 0.0, .z = -1.0},
        .radius = 0.5,
        .type = HitRecordType_Sphere
    };
    bn_daAppend(&world, sphere, &bn_ctx->allocator);

    // Camera
    f32 focal_length = 1.0;
    f32 viewport_height = 2.0;
    f32 viewport_width = viewport_height * ((f32)img_width / img_height);
    Point3 camera_center = {0};

    // Calculate the vectors across the horizontal and down the vertical
    // viewport edges.
    Vec3f32 viewport_u = {.x = viewport_width, .y = 0.0, .z = 0.0};
    Vec3f32 viewport_v = {.x = 0.0, .y = -viewport_height, .z = 0.0};

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    Vec3f32 pixel_delta_u = vec3f32DivScalar(viewport_u, img_width);
    Vec3f32 pixel_delta_v = vec3f32DivScalar(viewport_v, img_height);

    // Calculate the location of the upper left pixel.
    Vec3f32 viewport_upper_left = vec3f32Sub(
        vec3f32Sub(
            vec3f32Sub(camera_center, (Vec3f32){{0, 0, focal_length}}),
            vec3f32DivScalar(viewport_u, 2)
        ),
        vec3f32DivScalar(viewport_v, 2)
    );

    Vec3f32 pixel00_loc = vec3f32Add(
        viewport_upper_left,
        vec3f32MultScalar(vec3f32Add(pixel_delta_u, pixel_delta_v), 0.5)
    );

    // render
    // https://en.wikipedia.org/wiki/Netpbm
    FILE* f_ptr;
#if defined(BN_PLATFORM_WINDOWS)
    fopen_s(&f_ptr, "image.ppm", "w");
#else
    f_ptr = fopen("image.ppm", "w");
#endif

    fprintf(f_ptr, "P3\n");
    fprintf(f_ptr, "%d %d\n", img_width, img_height);
    fprintf(f_ptr, "255\n");

    for (i32 j = 0; j < img_height; j++) {
        bn_logInfof("Scanlines remaining: %d %d", (img_height - j));
        for (i32 i = 0; i < img_width; i++) {
            Vec3f32 pixel_center = vec3f32Add(
                vec3f32Add(pixel00_loc, vec3f32MultScalar(pixel_delta_u, i)),
                vec3f32MultScalar(pixel_delta_v, j)
            );
            Vec3f32 ray_direction = vec3f32Sub(pixel_center, camera_center);
            Ray r = {camera_center, ray_direction};

            Vec3f32 pixel_color = rayColor(&r, &world);
            writeColor(f_ptr, &pixel_color);
        }
    }
    fclose(f_ptr);

    bn_logInfof("Done");

    bn_destroyCtx();
    return 0;
}
