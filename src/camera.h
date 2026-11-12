#pragma once

#include "bn.h"
#include "utils.h"
#include "ray.h"

typedef struct {
    f32 aspect_ratio;
    i32 img_width, img_height;
    Point3 center, pixel00_loc;
    Vec3f32 pixel_delta_u, pixel_delta_v;
} Camera;

void cameraInit(Camera* camera) {
    if (camera->aspect_ratio <= 0)
        camera->aspect_ratio = 1.0;
    if (camera->img_width <= 0)
        camera->img_width = 100;

    camera->img_height = (i32)(camera->img_width / camera->aspect_ratio);
    camera->img_height = (camera->img_height < 1) ? 1 : camera->img_height;

    camera->center = (Point3){.x = 0.0, .y = 0.0, .z = 0.0};

    f32 focal_length = 1.0;
    f32 viewport_height = 2.0;
    f32 viewport_width =
        viewport_height * ((f32)camera->img_width / camera->img_height);

    // Calculate the vectors across the horizontal and down the vertical
    // viewport edges.
    Vec3f32 viewport_u = {.x = viewport_width, .y = 0.0, .z = 0.0};
    Vec3f32 viewport_v = {.x = 0.0, .y = -viewport_height, .z = 0.0};

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    camera->pixel_delta_u = vec3f32DivScalar(viewport_u, camera->img_width);
    camera->pixel_delta_v = vec3f32DivScalar(viewport_v, camera->img_height);

    // Calculate the location of the upper left pixel.
    Vec3f32 viewport_upper_left = vec3f32Sub(
        vec3f32Sub(
            vec3f32Sub(camera->center, (Vec3f32){{0, 0, focal_length}}),
            vec3f32DivScalar(viewport_u, 2)
        ),
        vec3f32DivScalar(viewport_v, 2)
    );

    camera->pixel00_loc = vec3f32Add(
        viewport_upper_left,
        vec3f32MultScalar(
            vec3f32Add(camera->pixel_delta_u, camera->pixel_delta_v), 0.5
        )
    );
}

Color cameraRayColor(Camera* camera, Ray* r, BN_arrayHitRecord* world) {
    (void)camera;
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

void cameraRender(Camera* camera, BN_arrayHitRecord* world) {
    cameraInit(camera);

    // render
    // https://en.wikipedia.org/wiki/Netpbm
    FILE* f_ptr;
#if defined(BN_PLATFORM_WINDOWS)
    fopen_s(&f_ptr, "image.ppm", "w");
#else
    f_ptr = fopen("image.ppm", "w");
#endif

    fprintf(f_ptr, "P3\n");
    fprintf(f_ptr, "%d %d\n", camera->img_width, camera->img_height);
    fprintf(f_ptr, "255\n");

    for (i32 j = 0; j < camera->img_height; j++) {
        i32 pct = (j * 100) / camera->img_height;
        bn_logInfof(
            "Scanlines remaining: %d (%d%%)", (camera->img_height - j), pct
        );
        for (i32 i = 0; i < camera->img_width; i++) {
            Vec3f32 pixel_center = vec3f32Add(
                vec3f32Add(
                    camera->pixel00_loc,
                    vec3f32MultScalar(camera->pixel_delta_u, i)
                ),
                vec3f32MultScalar(camera->pixel_delta_v, j)
            );
            Vec3f32 ray_direction = vec3f32Sub(pixel_center, camera->center);
            Ray r = {camera->center, ray_direction};

            Color pixel_color = cameraRayColor(camera, &r, world);
            writeColor(f_ptr, &pixel_color);
        }
    }
    bn_logInfof("Done");

    fclose(f_ptr);
}
