#include "./../camera/camera.h"
#include "./../material/material.h"

#include <yaml-cpp/yaml.h>
#include <fstream>

int checkargs(int argc, char* argv[]){
    if (argc <= 1 || (argc == 2 && std::string(argv[1]) == "-h")) {
        std::cerr << "\n\n\nUsage: raytracer [OPTION]... SOURCE FILE\n\n" << std::endl;
        std::cerr << "Optional arguments to configure iamge or camera." << std::endl;
        std::cerr << "  -ar  [double]                           Ratio of image width over height" << std::endl;
        std::cerr << "  -iw  [int]                              Rendered image width in pixel count" << std::endl;
        std::cerr << "  -spp [int]                              Number of rays sent into each pixel" << std::endl;
        std::cerr << "  -md  [int]                              Maximum number of ray bounces into the scene" << std::endl;
        std::cerr << "  -bg  [double] [double] [double]         Background color of the rendered scene" << std::endl;
        std::cerr << "                                           Represents the color seen behind objects in the scene." << std::endl;
        std::cerr << "  -vf  [double]                           Vertical field of view in degrees" << std::endl;
        std::cerr << "                                           Determines how much of the scene is visible vertically." << std::endl;
        std::cerr << "  -lf  [int] [int] [int]                  Camera's initial position in 3D space (x, y, z)" << std::endl;
        std::cerr << "                                           Defines where the camera is located within the scene." << std::endl;
        std::cerr << "  -la  [int] [int] [int]                  The point the camera is looking at in 3D space (x, y, z)" << std::endl;
        std::cerr << "                                           Specifies the point or object the camera is aimed towards." << std::endl;
        std::cerr << "  -vu  [int] [int] [int]                  Up direction of the camera in 3D space (x, y, z)" << std::endl;
        std::cerr << "                                           Sets the orientation of the camera's 'up' direction." << std::endl;
        std::cerr << "  -da [double]                            Angle used for defocusing (if applicable)" << std::endl;
        std::cerr << "                                           Determines the amount of blur in out-of-focus areas." << std::endl;
        std::cerr << "  -fd [double]                            Distance for focusing (if applicable)" << std::endl;
        std::cerr << "                                           Represents the distance at which objects are in sharp focus." << std::endl;

        return 0;
    }
    return 1;
}

void configurecamera(int argc, char* argv[], camera* cam){
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-ar" && i + 1 < argc) {
            cam->aspect_ratio = std::stod(argv[++i]);
        } else if (arg == "-iw" && i + 1 < argc) {
            cam->image_width = std::stoi(argv[++i]);
        } else if (arg == "-spp" && i + 1 < argc) {
            cam->samples_per_pixel = std::stoi(argv[++i]);
        } else if (arg == "-md" && i + 1 < argc) {
            cam->max_depth = std::stoi(argv[++i]);
        } else if (arg == "-bg" && i + 3 < argc) {
            double r = std::stod(argv[++i]);
            double g = std::stod(argv[++i]);
            double b = std::stod(argv[++i]);
            cam->background = color(r, g, b);
        } else if (arg == "-vf" && i + 1 < argc) {
            cam->vfov = std::stod(argv[++i]);
        } else if (arg == "-lf" && i + 2 < argc) {
            double x = std::stod(argv[++i]);
            double y = std::stod(argv[++i]);
            double z = std::stod(argv[++i]);
            cam->lookfrom = point3(x, y, z);
        } else if (arg == "-la" && i + 2 < argc) {
            double x = std::stod(argv[++i]);
            double y = std::stod(argv[++i]);
            double z = std::stod(argv[++i]);
            cam->lookat = point3(x, y, z);
        } else if (arg == "-vu" && i + 2 < argc) {
            double x = std::stod(argv[++i]);
            double y = std::stod(argv[++i]);
            double z = std::stod(argv[++i]);
            cam->vup = vec3(x, y, z);
        } else if (arg == "-da" && i + 1 < argc) {
            cam->defocus_angle = std::stod(argv[++i]);
        } else if (arg == "-fd" && i + 1 < argc) {
            cam->focus_dist = std::stod(argv[++i]);
        }
    }
}

void createscene(const std::string& filename, camera* cam, hittable_list* world){
    std::ifstream file(filename);
    if (!file.good()) {
        std::cerr << "Error: File '" << filename << "' does not exist or cannot be opened." << std::endl;
        return;
    }
    YAML::Node config = YAML::LoadFile(filename);
    std::map<std::string, std::shared_ptr<material>> materialsMap;

    cam->aspect_ratio = config["image"]["aspect_ratio"].as<double>();
    cam->image_width = config["image"]["image_width"].as<int>();
    cam->samples_per_pixel = config["image"]["samples_per_pixel"].as<int>();
    cam->max_depth = config["image"]["max_depth"].as<int>();
    std::vector<double> background = config["image"]["background"].as<std::vector<double>>();
    cam->background = color(background[0], background[1], background[2]);

    cam-> vfov = config["camera"]["vfov"].as<double>();
    std::vector<double> lookFrom = config["camera"]["look_from"].as<std::vector<double>>();
    cam->lookfrom = point3(lookFrom[0], lookFrom[1], lookFrom[2]);
    std::vector<double> lookAt = config["camera"]["look_at"].as<std::vector<double>>();
    cam->lookat = point3(lookAt[0], lookAt[1], lookAt[2]);
    std::vector<double> vup = config["camera"]["vup"].as<std::vector<double>>();
    cam->vup = vec3(vup[0], vup[1], vup[2]);

    cam->defocus_angle = config["depth_of_field"]["defocus_angle"].as<double>();
    cam->focus_dist = config["depth_of_field"]["focus_dist"].as<double>();

    for (const auto& material : config["materials"]) {
        std::string name = material.first.as<std::string>();
        std::string type = material.second["type"].as<std::string>();

        if (type == "lambertian") {
            auto colorValues = material.second["color"];
            auto lambertianMaterial = make_shared<lambertian>(color(colorValues[0].as<double>(), colorValues[1].as<double>(), colorValues[2].as<double>()));
            materialsMap[name] = lambertianMaterial;
        } else if (type == "metal") {
            auto colorValues = material.second["color"];
            double fuzziness = material.second["fuzziness"].as<double>();
            auto metalMaterial = make_shared<metal>(color(colorValues[0].as<double>(), colorValues[1].as<double>(), colorValues[2].as<double>()), fuzziness);
            materialsMap[name] = metalMaterial;
        } else if (type == "dielectric") {
            double indexOfRefraction = material.second["index_of_refraction"].as<double>();
            auto dielectricMaterial = make_shared<dielectric>(indexOfRefraction);
            materialsMap[name] = dielectricMaterial;
        } else if (type == "diffuse_light") {
            auto colorValues = material.second["color"];
            auto diffuseLightMaterial = make_shared<diffuse_light>(color(colorValues[0].as<double>(), colorValues[1].as<double>(), colorValues[2].as<double>()));
            materialsMap[name] = diffuseLightMaterial;
        }
    }

    for (const auto& obj : config["objects"]) {
        std::string type = obj["type"].as<std::string>();

        if (type == "quad") {
            auto parameters = obj["parameters"];
            point3 Q(parameters["Q"][0].as<double>(), parameters["Q"][1].as<double>(), parameters["Q"][2].as<double>());
            vec3 u(parameters["u"][0].as<double>(), parameters["u"][1].as<double>(), parameters["u"][2].as<double>());
            vec3 v(parameters["v"][0].as<double>(), parameters["v"][1].as<double>(), parameters["v"][2].as<double>());
            std::string materialName = parameters["material"].as<std::string>();

            world->add(make_shared<quad>(Q, u, v, materialsMap[materialName]));
        } else if (type == "box") {
            auto parameters = obj["parameters"];
            point3 a(parameters["a"][0].as<double>(), parameters["a"][1].as<double>(), parameters["a"][2].as<double>());
            point3 b(parameters["b"][0].as<double>(), parameters["b"][1].as<double>(), parameters["b"][2].as<double>());
            std::string materialName = parameters["material"].as<std::string>();

            world->add(box(a, b, materialsMap[materialName]));
        } else if (type == "sphere") {
            auto parameters = obj["parameters"];
            point3 center(parameters["center"][0].as<double>(), parameters["center"][1].as<double>(), parameters["center"][2].as<double>());
            double radius = parameters["radius"].as<double>();
            std::string materialName = parameters["material"].as<std::string>();

            world->add(make_shared<sphere>(center, radius, materialsMap[materialName]));
        }
    }
}