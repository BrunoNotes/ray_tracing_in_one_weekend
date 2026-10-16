#define BN_IMPLEMENTATION
#include "bn.h"

typedef Vec3f32 Point3;
typedef Vec3f32 Color;

void writeColor(FILE* f_ptr, Color pixel_color) {
    f32 r = pixel_color.r;
    f32 g = pixel_color.g;
    f32 b = pixel_color.b;

    i32 rb = (i32)(255.999 * r);
    i32 gb = (i32)(255.999 * g);
    i32 bb = (i32)(255.999 * b);

    fprintf(f_ptr, "%d %d %d\n", rb, gb, bb);
}

int main(void) {
    bn_initContext(BN_DEFAULT_CONTEXT_PARAMS);

    i32 img_width = 256;
    i32 img_height = 256;

    // https://en.wikipedia.org/wiki/Netpbm
    FILE* f_ptr;
    f_ptr = fopen("image.ppm", "w");

    fprintf(f_ptr, "P3\n");
    fprintf(f_ptr, "%d %d\n", img_width, img_height);
    fprintf(f_ptr, "255\n");

    for (i32 j = 0; j < img_height; j++) {
        bn_logInfof("Scanlines remaining: %d", (img_height - j));
        for (i32 i = 0; i < img_width; i++) {
            Vec3f32 pixel_color = {
                (f32)i / (img_width - 1),
                (f32)j / (img_height - 1),
                0.0,
            };

            writeColor(f_ptr, pixel_color);
        }
    }
    bn_logInfof("Done");

    fclose(f_ptr);

    bn_destroyContext();
    return 0;
}
