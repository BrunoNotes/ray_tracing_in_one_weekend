#define BN_IMPLEMENTATION
#include "bn.h"

typedef Vec3f32 Point3;
typedef Vec3f32 Color;

typedef struct {
    Point3 origin;
    Vec3f32 direction;
} Ray;

Point3 rayAt(Ray* ray, f32 t) {
    return vec3f32Add(ray->origin, vec3f32MultScalar(ray->direction, t));
}

Color rayColor(const Ray* r) {
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
    bn_initContext(BN_DEFAULT_CONTEXT_PARAMS);

    // image
    f32 aspect_ratio = 16.0 / 9.0;
    i32 img_width = 400;

    // Calculate the image height, and ensure that it's at least 1.
    i32 img_height = (i32)(img_width / aspect_ratio);
    img_height = (img_height < 1) ? 1 : img_height;

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
    f_ptr = fopen("image.ppm", "w");

    fprintf(f_ptr, "P3\n");
    fprintf(f_ptr, "%d %d\n", img_width, img_height);
    fprintf(f_ptr, "255\n");

    for (i32 j = 0; j < img_height; j++) {
        bn_logInfof("Scanlines remaining: %d", (img_height - j));
        for (i32 i = 0; i < img_width; i++) {
            Vec3f32 pixel_center = vec3f32Add(
                vec3f32Add(pixel00_loc, vec3f32MultScalar(pixel_delta_u, i)),
                vec3f32MultScalar(pixel_delta_v, j)
            );
            Vec3f32 ray_direction = vec3f32Sub(pixel_center, camera_center);
            Ray r = {camera_center, ray_direction};

            Vec3f32 pixel_color = rayColor(&r);

            writeColor(f_ptr, &pixel_color);
        }
    }
    bn_logInfof("Done");

    fclose(f_ptr);

    bn_destroyContext();
    return 0;
}
