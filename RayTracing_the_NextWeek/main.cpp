#include <fstream>
#include <iostream>
#include <cmath>

#include "scenes.h"
#include "camera.h"
#include "rtweekend.h"

vec3 ray_color(const ray &, const hittable &, int);


int main() {
	const int image_height = 400;
	const int image_width = 2 * image_height;
	const int samples_per_pixel = 100;
	const int max_depth = 50;

	std::ofstream outfile;
	outfile.open("Test.ppm");

	outfile << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	// hittable_list world = random_scene(5);
	// hittable_list world = two_perlin_spheres();
	hittable_list world = earth();

	const double fov = 20;
	const auto aspect_ratio = double(image_width) / image_height;
	vec3 lookfrom(13, 2, 3);
	vec3 lookat(0, 0, 0);
	vec3 vup(0, 1, 0);
	auto dist_to_focus = 10.0;
	auto aperture = 0.1;

	// camera cam
	camera cam(lookfrom, lookat, vup, fov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

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
	system("pause");
	return 0;
}


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
