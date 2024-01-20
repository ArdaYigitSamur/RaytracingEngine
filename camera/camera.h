#ifndef CAMERA_H
#define CAMERA_H

#include "./../headers/common.h"

#include "./../headers/color.h"
#include "./../headers/hittable.h"
#include "./../material/material.h"
#include <iostream>
#include <omp.h>
#include <vector>
#include <atomic>
#include <sstream>

class camera {
  public:
    double aspect_ratio = 1.0;  
    int    image_width  = 100;  
    int    samples_per_pixel = 50;
    int    max_depth    = 10;   
    color  background;
    
    double vfov = 90;
    point3 lookfrom = point3(0,0,-1);  
    point3 lookat   = point3(0,0,0);   
    vec3   vup      = vec3(0,-1,0);     

    double defocus_angle = 0;  
    double focus_dist = 10;    

    void render(const hittable& world) {
        
        initialize();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
        std::vector<color> image(image_width * image_height);

        const int NUM_TILES_X = 16;
        const int NUM_TILES_Y = 16;
        
        const int TILE_SIZE_X = image_width / NUM_TILES_X;
        const int TILE_SIZE_Y = image_height / NUM_TILES_Y;

        std::atomic<int> processedTiles(0);
        
        #pragma omp parallel for schedule(dynamic) num_threads(16)
        for (int tile_y = 0; tile_y < NUM_TILES_Y; tile_y++) {
            for (int tile_x = 0; tile_x < NUM_TILES_X; tile_x++) {
                
                int x0 = tile_x * TILE_SIZE_X;
                int y0 = tile_y * TILE_SIZE_Y;
                
                int x1 = x0 + TILE_SIZE_X;
                int y1 = y0 + TILE_SIZE_Y;
               
                for (int j = y0; j < y1; j++) {
                    for (int i = x0; i < x1; i++) {
                        color pixel_color(0,0,0);
                        for (int sample = 0; sample < samples_per_pixel; ++sample) {
                            ray r = get_ray(i, j);
                            pixel_color += ray_color(r, max_depth, world, color(1,1,1));
                        }                       
                        image[j * image_width + i] = pixel_color;
                    }
                }
                int processed = ++processedTiles;
                std::stringstream ss;
                ss << "\rProcessed " << processed << " out of " << (NUM_TILES_X * NUM_TILES_Y) << " tiles.";
                std::clog << ss.str() << std::flush;            
            }
        }
        std::clog << std::endl;
        
        for (int j = 0; j < image_height; ++j) {
            for (int i = 0; i < image_width; ++i) {
                write_color(image[j * image_width + i], samples_per_pixel);
            }
        }
    }

  private:
    int    image_height;   
    point3 center;         
    point3 pixel00_loc;    
    vec3   pixel_delta_u;  
    vec3   pixel_delta_v;  
    vec3   u, v, w;        
    vec3   defocus_disk_u;  
    vec3   defocus_disk_v;  

    void initialize() {

        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        center = lookfrom;
        
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta/2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * (static_cast<double>(image_width)/image_height);
        
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);
        
        vec3 viewport_u = viewport_width * u;    
        vec3 viewport_v = viewport_height * -v;  
        
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;
        
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
        
        auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;

    }

    ray get_ray(int i, int j) const {
        
        auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        auto pixel_sample = pixel_center + pixel_sample_square();

        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    point3 defocus_disk_sample() const {
        
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    vec3 pixel_sample_square() const {
        
        auto px = -0.5 + random_double();
        auto py = -0.5 + random_double();
        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }

    color ray_color(const ray& r, int depth, const hittable& world, color current_attenuation) const {
        
        hit_record rec;
        
        if (depth <= 0)
            return color(0,0,0);
        
        if (!world.hit(r, interval(0.001, infinity), rec))
            return background;

        ray scattered;
        color attenuation;
        color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

        if (!rec.mat->scatter(r, rec, attenuation, scattered))
            return color_from_emission;

        
        if (attenuation.length_squared() < 0.001)
            return color_from_emission;

        color color_from_scatter = attenuation * ray_color(scattered, depth-1, world, current_attenuation * attenuation);

        return color_from_emission + color_from_scatter;
    }
};

#endif