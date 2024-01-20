#include "headers/common.h"
#include "headers/color.h"
#include "headers/hittable_list.h"
#include "headers/sphere.h"
#include "headers/quad.h"
#include "headers/bvh.h"
#include "headers/parser.h"
#include "camera/camera.h"
#include "material/material.h"

#include <iostream>
#include <cstdlib>
#include <chrono>
#include <iomanip>

int main(int argc, char *argv[]) {
    
    if(!checkargs(argc, argv))
        return 0;

    hittable_list world;
    camera cam;

    createscene(argv[argc - 1], &cam, &world);
    configurecamera(argc, argv, &cam);
    
    cam.render(world);
}