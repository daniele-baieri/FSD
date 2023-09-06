#pragma once

#include <random>
#include <map>
#include <nlohmann/json.hpp>
#include <utils/utilities.hpp>


enum VarType {
    linscale,
    uniform,
    normal,
    collection,
    dam,
    cuboid,
    sphere,
    geometry
};
const std::map<const char*, VarType> VarTypeMap {
    {"linscale", VarType::linscale},
    {"uniform", VarType::uniform},
    {"normal", VarType::normal},
    {"collection", VarType::collection},
    {"dam", VarType::dam},
    {"cuboid", VarType::cuboid},
    {"sphere", VarType::sphere},
    {"geometry", VarType::geometry},
};
const std::random_device rd;
const std::mt19937 gen(rd());


struct Cuboid {

    const float3 center;
    const float3 sides;
    const float3 rotation;

    Cuboid(const float3 center, const float3 sides, const float3 rotation) :
        center(center), sides(sides), rotation(rotation) {}

};

struct Sphere {

    const float3 center;
    const float radius;

    Sphere(const float3 center, const float radius) :
        center(center), radius(radius) {}

};

struct Geometry {

    const std::string asset;
    const float3 center;
    const float3 scale;
    const float3 rotation;
    const float size;

    Geometry(const std::string& asset, const float3 center, const float3 scale, const float3 rotation, const float size) :
        center(center), scale(scale), rotation(rotation), asset(asset), size(size) {}

};

template<typename T>
void sample_collection(std::vector<T>& out, const std::vector<T>& collection) {
    for (auto it = collection.begin(); it != collection.end(); it++) {
        out.push_back(*it);
    }
}

void sample_linscale(std::vector<int>& out, const int& vmin, const int& vmax, const uint step) {
    for (int i = vmin; i < vmax; i += step) {
        out.push_back(i);
    }
    out.push_back(vmax);
}

void sample_linscale(std::vector<float>& out, const float& vmin, const float& vmax, const uint nsteps) {
    float diff = vmax - vmin;
    float step = diff / (float)nsteps;
    float val = vmin;
    for (int i = 0; i < nsteps; i++) {
        out.push_back(val);
        val += step;
    }
}

void sample_normal(std::vector<float>& out, const float mean, const float std, const uint nsamples) {
    std::normal_distribution<float> dist(mean, std);
    for (int i = 0; i < nsamples; i++) {
        out.push_back(dist(gen));
    }
}

void sample_uniform(std::vector<float>& out, const float lo, const float hi, const uint nsamples) {
    std::uniform_real_distribution<float> dist(lo, hi);
    for (int i = 0; i < nsamples; i++) {
        out.push_back(dist(gen));
    }
}

void sample_cuboid(
    std::vector<Cuboid>& out, const uint3 N, const uint min_side_length, 
    const uint max_side_length, const uint nsamples
) {

    std::uniform_int_distribution<uint> side_dist(min_side_length, N.x);

    uint half_side_l = floor((float)min_side_length / 2.0f);
    std::uniform_int_distribution<uint> xdist(half_side_l, N.x);
    std::uniform_int_distribution<uint> ydist(1u, N.y);
    std::uniform_int_distribution<uint> zdist;
    if (on_ground) {
        zdist = std::uniform_int_distribution<uint>(min_ctr_z, floor((float)N.z / 2.0f) - 1);
    } else {
        zdist = std::uniform_int_distribution<uint>(min_ctr_z, N.z - min_ctr_z);
    }


    float3 center, sides, rotation;
    for (int i = 0; i < nsamples; i++) {
        sides = {side_dist(gen), side_dist(gen), side_dist(gen)};
        center = {xdist(gen), ydist(gen), zdist(gen)};
        out.emplace_back(center, sides, rotation);
    }
    
}

void sample_sphere(std::vector<Sphere>& out, const uint3 N, bool on_ground, const uint nsamples) {
    std::uniform_int_distribution<uint> dist(???);

}

void sample_geometry(std::vector<Geometry>& out, const uint3 N, bool on_ground, const uint nsamples) {
    std::uniform_int_distribution<uint> dist(???);

}



template<typename T>
class VariableSampler {

private:

    const VarType vtype;

    std::vector<T> samples;

public:
    
    VariableSampler(const VarType type) : vtype(type) {}

    template<typename... Args>
    void sample(const Args... args) {
        if (samples.size() > 0)
            samples.clear();
        switch(this->vtype) {
            case VarType::linscale:
                sample_linscale(samples, args...);
                break;
            case VarType::uniform:
                sample_uniform(samples, args...);
                break;
            case VarType::normal:
                sample_normal(samples, args...);
                break;
            case VarType::collection:
                sample_collection(samples, args...);
                break;
            case VarType::cuboid:
                sample_cuboid(samples, args...);
                break;
            case VarType::sphere:
                sample_sphere(samples, args...);
                break;
            case VarType::geometry:
                sample_geometry(samples, args...);
                break;
        }
    }

};