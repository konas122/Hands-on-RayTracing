#ifndef RAY_H
#define RAY_H

#include "vec3.h"


class ray {
public:
	ray() {}
	ray(const vec3& orign, const vec3& direction)
		: orig(orign), dir(direction) {}

	vec3 origin() const { return orig; }
	vec3 direction() const { return dir; }

	vec3 at(double t) const {
		return orig + t * dir;
	}

public:
	vec3 orig;
	vec3 dir;
};

#endif