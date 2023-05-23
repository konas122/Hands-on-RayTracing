#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"
#include "rtweekend.h"


double schlick(double cosine, double ref_idx) {
    auto r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}


class material {
    public:
        virtual bool scatter(
            const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered) const = 0;
};


class lambertian : public material {
    public:
        vec3 albedo;

    public:
    lambertian(const vec3& a) : albedo(a) {}

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered
    ) const {
        vec3 scatter_direction = rec.normal + random_unit_vector(); //random_in_unit_sphere() or random_in_hemisphere()
        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }
};


class metal : public material {
    public:
        vec3 albedo;
        double fuzz;

    public:
    metal(const vec3& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered
    ) const {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
        attenuation = albedo; 	// reflectance of the material.
        return (dot(scattered.direction(), rec.normal) > 0);    //if dot < 0 absorb light
    }
};


class dielectric : public material {
    public:
        double ref_idx; 
    
    public:
    dielectric(double ri) : ref_idx(ri) {}

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered
    ) const {
        attenuation = vec3(1.0, 1.0, 1.0);
        double etai_over_etat;
        if (rec.front_face) 
            etai_over_etat = 1.0 / ref_idx;
        else
            etai_over_etat = ref_idx;

        vec3 unit_direction = unit_vector(r_in.direction());
        double costh = ffmin(dot(unit_direction, rec.normal), 1.0);
        double sinth = sqrt(1.0 - costh * costh);

        if (etai_over_etat * sinth > 1.0) {
            vec3 reflected = reflect(unit_direction, rec.normal);
            scattered = ray(rec.p, reflected);
            return true;
        }

        double reflect_prob = schlick(costh, etai_over_etat);
        if (random_double() < reflect_prob) {
            vec3 reflected = reflect(unit_direction, rec.normal);
            scattered = ray(rec.p, reflected);
            return true;
        }
        
        vec3 refracted = refract(unit_direction, rec.normal, etai_over_etat);
        scattered = ray(rec.p, refracted);
        return true;
    }
};

#endif
