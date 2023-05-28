#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"
#include "aabb.h"


class material;

struct hit_record {
    vec3 p;         // point coordinates
    vec3 normal;    // normal vector
    double t;
    bool front_face;

    shared_ptr<material> mat_ptr;

    double u, v;    // used for texture

    inline void set_face_normal(const ray& r, const vec3& outward_nomal) {
        front_face = dot(r.direction(), outward_nomal) < 0;
        normal = front_face ? outward_nomal : -outward_nomal;
    }
};


class hittable {
    public:

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;

    virtual bool bounding_box(double t0, double t1, aabb& output_box) const = 0;
};

#endif
