#include <fstream>
#include <iostream>
#include <cmath>

#include "scenes.h"
#include "camera.h"
#include "rtweekend.h"

vec3 ray_color(const ray &, const hittable &, int);
vec3 ray_color(const ray &r, const vec3 &background, const hittable &world, int depth);


int main() {
	const int image_height = 200;
	const int image_width = 1 * image_height; // k * image_height, you can change k yourself
	const int samples_per_pixel = 3000;
	const int max_depth = 100;

	std::ofstream outfile;
	outfile.open("Test.ppm");

	outfile << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	// hittable_list world = random_scene(5);
	// hittable_list world = two_perlin_spheres();
	// hittable_list world = earth();
	// hittable_list world = simple_light();
	// hittable_list world = cornell_box_0();
	// hittable_list world = cornell_box();
	// hittable_list world = cornell_smoke();
	hittable_list world = final_scene();

	const double fov = 30;
	const auto aspect_ratio = double(image_width) / image_height;

	/* The following is the camera view, or you can change it yourself */
	/* used for other render tasks except conrell box and final_scene */
	// vec3 lookfrom(13, 2, 3);
	// vec3 lookat(0, 0, 0);
	// vec3 vup(0, 1, 0);
	// auto dist_to_focus = 10.0;
	// auto aperture = 0.1;

	/* used for rendering cornell box */
	// vec3 lookfrom(278, 278, -800);
	// vec3 lookat(278, 278, 0);
	// vec3 vup(0, 1, 0);
	// auto dist_to_focus = 10.0;
	// auto aperture = 0.0;

	/* used for rendering final scene */
	vec3 lookfrom(408, 278, -800);
	vec3 lookat(278, 278, 0);
	vec3 vup(0, 1, 0);
	auto dist_to_focus = 10.0;
	auto aperture = 0.0;

	auto vfov = 40.0;

	// camera 
	camera cam(lookfrom, lookat, vup, fov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

	const vec3 background(0, 0, 0);

	for (int j = image_height - 1; j >= 0; --j) {
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
		for (int i = 0; i < image_width; ++i) {
			vec3 color(0, 0, 0);
			for (int s = 0; s < samples_per_pixel; ++s) {
				auto u = (i + random_double()) / image_width;
				auto v = (j + random_double()) / image_height;
				ray r = cam.get_ray(u, v);
				// color += ray_color(r, world, max_depth);				// with light source background
				color += ray_color(r, background, world, max_depth);	// without light source background
			}
			color.write_color(outfile, samples_per_pixel);
		}
	}

	std::cerr << "\nDone.\n";
	system("pause");
	return 0;
}


vec3 ray_color(const ray& r, const hittable& world, int depth) {
	// If we've exceeded the ray bounce limit, no more light is gathered.
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


vec3 ray_color(const ray& r, const vec3& background, const hittable& world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
		return vec3(0, 0, 0);

	// If the ray hits nothing, return the background color.
    if (!world.hit(r, 0.001, infinity, rec))
        return background;

    ray scattered;
    vec3 attenuation;
    vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return emitted;

    return emitted + attenuation * ray_color(scattered, background, world, depth-1);
}
