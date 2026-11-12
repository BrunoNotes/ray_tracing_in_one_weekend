#define BN_IMPLEMENTATION
#include "bn.h"

#include "utils.h"
#include "interval.h"
#include "ray.h"
#include "camera.h"

/*
   https://raytracing.github.io/books/RayTracingInOneWeekend.html

   stoped at: 8. Antialiasing
*/

int main(void) {
    BN_Allocator allocator = bn_initAllocator((BN_AllocatorInitParams) {
        .type = BN_AllocatorType_ArenaGrowing,
    });

    // World
    BN_arrayHitRecord world = bn_daInit();

    HitRecord sphere = (HitRecord){
        .center = (Point3){.x = 0.0, .y = 0.0, .z = -1.0},
        .radius = 0.5,
        .type = HitRecordType_Sphere
    };
    bn_daAppend(&world, sphere, &allocator);

    sphere = (HitRecord){
        .center = (Point3){.x = 0.0, .y = -100.5, .z = -1.0},
        .radius = 100.0,
        .type = HitRecordType_Sphere
    };
    bn_daAppend(&world, sphere, &allocator);

    Camera cam = {0};

    cam.aspect_ratio = 16.0 / 9.0;
    cam.img_width = 400;

    cameraRender(&cam, &world);

    bn_destroyAllocator(&allocator);
    return 0;
}
