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
    fixed,
    dam,
    cuboid,
    sphere,
    //geometry
};
const std::map<const char*, VarType> VarTypeMap {
    {"linscale", VarType::linscale},
    {"uniform", VarType::uniform},
    {"normal", VarType::normal},
    {"collection", VarType::collection},
    {"fixed", VarType::fixed},
    {"dam", VarType::dam},
    {"cuboid", VarType::cuboid},
    {"sphere", VarType::sphere},
    //{"geometry", VarType::geometry},
};
const std::random_device rd;
const std::mt19937 gen(rd());

using uint2 = std::pair<uint, uint>;
using float2 = std::pair<float, float>;



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
/*
struct Geometry {

    const std::string asset;
    const float3 center;
    const float3 scale;
    const float3 rotation;
    const float size;

    Geometry(const std::string& asset, const float3 center, const float3 scale, const float3 rotation, const float size) :
        center(center), scale(scale), rotation(rotation), asset(asset), size(size) {}

};
*/

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
    std::vector<Cuboid>& out, const uint3 N, 
    const uint2 cx_range, const uint2 cy_range, const uint2 cz_range, 
    const uint2 sx_range, const uint2 sy_range, const uint2 sz_range, 
    const float2 rx_range, const float2 ry_range, const float2 rz_range, 
    const bool on_ground, const uint nsamples
) {

    std::uniform_int_distribution<uint> adist(sx_range.first, sx_range.second);
    std::uniform_int_distribution<uint> bdist(sy_range.first, sy_range.second);
    std::uniform_int_distribution<uint> cdist(sz_range.first, sz_range.second);

    std::uniform_int_distribution<uint> xdist(cx_range.first, cx_range.second);
    std::uniform_int_distribution<uint> ydist(cy_range.first, cy_range.second);
    std::uniform_int_distribution<uint> zdist;
    if (on_ground) {
        zdist = std::uniform_int_distribution<uint>(cz_range.first, min(cz_range.second, floor((float)N.z/2.0f)));
    } else {
        zdist = std::uniform_int_distribution<uint>(cz_range.first, cz_range.second);
    }

    std::uniform_int_distribution<float> rxdist(rx_range.first, rx_range.second);
    std::uniform_int_distribution<float> rydist(ry_range.first, ry_range.second);
    std::uniform_int_distribution<float> rzdist(rz_range.first, rz_range.second);

    //uint x, y, z, a, b, c;
    float3 center, sides, rotation;
    for (int i = 0; i < nsamples; i++) {
        center = {xdist(gen), ydist(gen), zdist(gen)};
        if (on_ground) {
            cdist = std::uniform_int_distribution<uint> cdist(center.z * 2u, center.z * 2u);
        } 
        sides = {adist(gen), bdist(gen), cdist(gen)};
        rotation = {rxdist(gen), rydist(gen), rzdist(gen)};
        out.emplace_back(center, sides, rotation);
    }
    
}

void sample_sphere(
    std::vector<Sphere>& out, const uint3 N, 
    const uint2 cx_range, const uint2 cy_range, const uint2 cz_range, 
    const float2 rad_range, const bool on_ground, const uint nsamples) 
{

    std::uniform_int_distribution<uint> rdist(sz_range.first, sz_range.second);

    std::uniform_int_distribution<uint> xdist(cx_range.first, cx_range.second);
    std::uniform_int_distribution<uint> ydist(cy_range.first, cy_range.second);
    std::uniform_int_distribution<uint> zdist;
    if (on_ground) {
        zdist = std::uniform_int_distribution<uint>(cz_range.first, min(cz_range.second, floor((float)N.z/2.0f)));
    } else {
        zdist = std::uniform_int_distribution<uint>(cz_range.first, cz_range.second);
    }

    //uint x, y, z, a, b, c;
    float3 center;
    float radius;
    for (int i = 0; i < nsamples; i++) {
        center = {xdist(gen), ydist(gen), zdist(gen)};
        if (on_ground) {
            rdist = std::uniform_int_distribution<uint> cdist(center.z, center.z);
        } 
        radius = rdist(gen);
        out.emplace_back(center, radius);
    }

}
/*
void sample_geometry(std::vector<Geometry>& out, const uint3 N, bool on_ground, const uint nsamples) {
    //std::uniform_int_distribution<uint> dist(???);

}
*/


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
            /*case VarType::geometry:
                sample_geometry(samples, args...);
                break;*/
        }
    }

};