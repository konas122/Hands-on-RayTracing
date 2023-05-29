#ifndef SCENES_H
#define SCENES_H

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "bvh.h"
#include "box.h"
#include "aarect.h"
#include "sphere.h"
#include "material.h"
#include "rtweekend.h"
#include "hittable_list.h"


hittable_list random_scene(int num = 5) {
	hittable_list world;

	auto checker = make_shared<checker_texture>(
    	make_shared<constant_texture>(vec3(0.2, 0.3, 0.1)),
    	make_shared<constant_texture>(vec3(0.9, 0.9, 0.9))
	);

	world.add(
		make_shared<sphere>(
			vec3(0, -1000, 0), 1000,
			make_shared<lambertian>(checker)
		)
	);

	for (int a = -num; a < num; a++) {
		for (int b = -num; b < num; b++) {
			auto choose_mat = random_double();
			vec3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
			
			if (choose_mat < 0.8) {	// diffuse
                auto albedo = vec3::random() * vec3::random();
                world.add(
					make_shared<moving_sphere>(
                    	center, center + vec3(0, random_double(0,.5), 0), 
						0.0, 1.0, 0.2,
                    	make_shared<lambertian>(
							make_shared<constant_texture>(albedo)
						)
					)
				);
			}
			else if (choose_mat < 0.95) { //metal
				auto albedo = vec3::random(.5, 1);
				auto fuzz = random_double(0, .5);
				world.add(
					make_shared<sphere>(
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
				make_shared<constant_texture>(vec3(0.4, 0.2, 0.1))
			)
		)
	);
	world.add(
		make_shared<sphere>(
			vec3(4, 1, 0), 1.0,
			make_shared<metal>(vec3(0.7, 0.6, 0.5), 0.0)
		)
	);
	// return world;
	return static_cast<hittable_list>(make_shared<bvh_node>(world, 0, 1));
}


hittable_list two_perlin_spheres() {
    hittable_list world;

    auto pertext = make_shared<noise_texture>();
    world.add(
        make_shared<sphere>(
            vec3(0, -1000, 0),
            1000,
            make_shared<lambertian>(pertext)
        )
    );
    world.add(
        make_shared<sphere>(
            vec3(0, 2, 0),
            2,
            make_shared<lambertian>(pertext)
        )
    );
    return world;
}


hittable_list earth() {
    int nx, ny, nn;
    unsigned char* texture_data = stbi_load("earthmap.jpg", &nx, &ny, &nn, 0);

	auto earth_surface =
		make_shared<lambertian>(
			make_shared<image_texture>(texture_data, nx, ny)
		);
	auto globe = make_shared<sphere>(
		vec3(0, 0, 0), 2, earth_surface
	);

	return hittable_list(globe);
}


hittable_list simple_light() {
    hittable_list objects;

	auto pertext = make_shared<noise_texture>(4);
	objects.add(
		make_shared<sphere>(
			vec3(0, -1000, 0), 1000, 
			make_shared<lambertian>(pertext)
		)
	);
	objects.add(
		make_shared<sphere>(
			vec3(0, 2, 0), 2, 
			make_shared<lambertian>(pertext)
		)
	);

	auto difflight = make_shared<diffuse_light>(
		make_shared<constant_texture>(vec3(4, 4, 4))
	);
	objects.add(
		make_shared<sphere>(
			vec3(0, 7, 0), 2, difflight
		)
	);
	objects.add(
		make_shared<xy_rect>(1, 5, 0, 4, -4, difflight)
	);

	return objects;
}


hittable_list cornell_box_0() {
	hittable_list world;

	auto red = make_shared<lambertian>(
		make_shared<constant_texture>(
			vec3(0.65, 0.05, 0.05)
		)
	);
	auto white = make_shared<lambertian>(
		make_shared<constant_texture>(
			vec3(0.73, 0.73, 0.73)
		)
	);
	auto green = make_shared<lambertian>(
		make_shared<constant_texture>(
			vec3(0.12, 0.45, 0.15)
		)
	);
	auto light = make_shared<diffuse_light>(
		make_shared<constant_texture>(
			vec3(15, 15, 15)
		)
	);

	world.add(
		make_shared<flip_face>(
			make_shared<yz_rect>(0, 555, 0, 555, 555, green)
		)
	);
	world.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
	world.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
	world.add(
		make_shared<flip_face>(
			make_shared<xy_rect>(0, 555, 0, 555, 555, white)
		)
	);
	world.add(
		make_shared<flip_face>(
			make_shared<xz_rect>(0, 555, 0, 555, 0, white)
		)
	);
	world.add(
		make_shared<flip_face>(
			make_shared<xz_rect>(0, 555, 0, 555, 555, white)
		)
	);

	world.add(
		make_shared<box>(
			vec3(130, 0, 65), vec3(295, 165, 230), white
		)
	);
	world.add(
		make_shared<box>(
			vec3(265, 0, 295), vec3(430, 330, 460), white
		)
	);
	return world;
	// return static_cast<hittable_list>(make_shared<bvh_node>(world, 0, 1));
}


hittable_list cornell_box() {
	hittable_list objects;

	auto red = make_shared<lambertian>(
		make_shared<constant_texture>(
			vec3(0.65, 0.05, 0.05)
		)
	);
	auto white = make_shared<lambertian>(
		make_shared<constant_texture>(
			vec3(0.73, 0.73, 0.73)
		)
	);
	auto green = make_shared<lambertian>(
		make_shared<constant_texture>(
			vec3(0.12, 0.45, 0.15)
		)
	);
	auto light = make_shared<diffuse_light>(
		make_shared<constant_texture>(
			vec3(15, 15, 15)
		)
	);

	objects.add(
		make_shared<flip_face>(
			make_shared<yz_rect>(0, 555, 0, 555, 555, green)
		)
	);
	objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
	objects.add(make_shared<xz_rect>(22, 343, 300, 332, 554, light));
	objects.add(
		make_shared<flip_face>(
			make_shared<xy_rect>(0, 555, 0, 555, 555, white)
		)
	);
	objects.add(
		make_shared<flip_face>(
			make_shared<xz_rect>(0, 555, 0, 555, 0, white)
		)
	);
	objects.add(
		make_shared<flip_face>(
			make_shared<xz_rect>(0, 555, 0, 555, 555, white)
		)
	);

	// objects.add(
	// 	make_shared<box>(
	// 		vec3(130, 0, 65), vec3(295, 165, 230), white
	// 	)
	// );
	// objects.add(
	// 	make_shared<box>(
	// 		vec3(265, 0, 295), vec3(430, 330, 460), white
	// 	)
	// );

	// shared_ptr<hittable> box1 = make_shared<box>(vec3(130, 0, 65), vec3(295, 165, 230), white);
	// box1 = make_shared<rotate_y>(box1, 15);
	// box1 = make_shared<translate>(box1, vec3(265, 0, 295));
	// objects.add(box1);

	shared_ptr<hittable> box2 = make_shared<box>(vec3(265, 0, 295), vec3(430, 330, 460), white);
	box2 = make_shared<rotate_y>(box2, -18);
	box2 = make_shared<translate>(box2, vec3(130, 0, 65));
	objects.add(box2);

	return objects;
	// return static_cast<hittable_list>(make_shared<bvh_node>(objects, 0, 1));
}

#endif
