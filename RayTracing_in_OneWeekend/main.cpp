#include <fstream>
#include <iostream>
#include <cmath>

#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "hittable.h"
#include "rtweekend.h"
#include "hittable_list.h"

hittable_list random_scene(int num);

vec3 ray_color(const ray& r, const hittable& world, int depth) {
	if (depth <= 0)
		return vec3(0, 0, 0);
	
	hit_record rec;
    if (world.hit(r, 0.001, infinity, rec)) {
		ray scattered;
		vec3 attenuation;
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
			return attenuation * ray_color(scattered, world, depth - 1);
		return vec3(0, 0, 0);
	}
	
	vec3 unit_direction = unit_vector(r.direction());
	auto t = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}


int main() {
	const int image_height = 200;
	const int image_width = 2 * image_height;
	const int samples_per_pixel = 100;
	const int max_depth = 50; 

	std::ofstream outfile;
	outfile.open("Test.ppm");

	outfile << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	hittable_list world = random_scene(10);

	// camera cam;
	const double fov = 20;
	const auto aspect_ratio = double(image_width) / image_height;
	vec3 lookfrom(13, 3, 2);
	vec3 lookat(0, 0, 0);
	vec3 vup(0, 1, 0);
	auto dist_to_focus = 10.0;
	auto aperture = 0.1;

	// camera cam;
	camera cam(lookfrom, lookat, vup, fov, aspect_ratio, aperture, dist_to_focus);

	for (int j = image_height - 1; j >= 0; --j) {
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
		for (int i = 0; i < image_width; ++i) {
			vec3 color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / image_width;
                auto v = (j + random_double()) / image_height;
                ray r = cam.get_ray(u, v);
                color += ray_color(r, world, max_depth);
            }
            color.write_color(outfile, samples_per_pixel);
		}
	}
	
	std::cerr << "\nDone.\n";
	return 0;
}


hittable_list random_scene(int num = 11) {
    hittable_list world;

    world.add(
        make_shared<sphere>(
            vec3(0, -1000, 0), 1000,
            make_shared<lambertian>(
                vec3(0.5, 0.5, 0.5)
            )
        )
    );

    for (int a = -num; a < num; a++) {
        for (int b = -num; b < num; b++) {
            auto choose_mat = random_double();
            vec3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
            if (choose_mat < 0.8) { // diffuse 
                auto albedo = vec3::random() * vec3::random();
                world.add(
                    make_shared<sphere>(
                        center, 0.2,
                        make_shared<lambertian>(albedo)
                    )
                );
            }
            else if (choose_mat < 0.95) { //metal
                auto albedo = vec3::random(.5, 1);
                auto fuzz = random_double(0, .5);
                world.add(make_shared<sphere>(
                    center, 0.2,
                    make_shared<metal>(albedo, fuzz)
                    )
                );
            }
            else { //glass
                world.add(
                    make_shared<sphere>(
                        center, 0.2,
                        make_shared<dielectric>(1.5)
                    )
                );
            }
        }
    }

    world.add(
        make_shared<sphere>(
            vec3(0, 1, 0), 1.0,
            make_shared<dielectric>(1.5)
        )
    );
    world.add(
        make_shared<sphere>(
            vec3(-4, 1, 0), 1.0, 
            make_shared<lambertian>(
                vec3(0.4, 0.2, 0.1)
            )
        )
    );
    world.add(
        make_shared<sphere>(
            vec3(4, 1, 0), 1.0,
            make_shared<metal>(
                vec3(0.7, 0.6, 0.5), 0.0)
            )
        );
	return world;
}
