#include <iostream>
#include <string>
#include <filesystem>
#include <map>
#include <random>
#include <stdlib.h>
#include <fx3d/lbm.hpp>
#include <fx3d/settings.hpp>
#include <fx3d/scenes.hpp>
#include <fx3d/info.hpp>
#include "dataset_scene.hpp"




fx3d::Info fx3d::info;
Units units;
const std::mt19937 rand_eng;


enum VarType {
    constant,
    linscale,
    uniform,
    normal,
};
const std::map<const char*, VarType> VarTypeMap {
    {"constant", VarType::constant},
    {"linscale", VarType::linscale},
    {"uniform", VarType::uniform},
    {"normal", VarType::normal},
};


template<typename T>
std::vector<T> sample_constant(const T& value) {

}

template<typename T>
std::vector<T> sample_linscale(const T& vmin, const T& vmax, const int nsteps) {

}

std::vector<float> sample_uniform(const float lo, const float hi, const int nsamples) {

}

template<typename T>
std::vector<T> sample_uniform(const std::vector<T>& collection, const int nsamples) {

}

template<typename T>
std::vector<T> sample_normal(const float mu, const float sigma, const int nsamples) {
    
}

template<typename T>
std::vector<T> sample_variable(const nlohmann::json &config) {
    
}




void make_scenes(const nlohmann::json &config) {

}

void generate(const std::filesystem::path& root, const std::ofstream& log) {

}



int main(int argc, char** argv) {

    std::ifstream stream(argv[1]);
    nlohmann::json config = nlohmann::json::parse(stream);

    make_scenes(config);

    
    fx3d::Settings::EnableFeature(fx3d::Feature::VOLUME_FORCE);
    fx3d::Settings::EnableFeature(fx3d::Feature::SURFACE);

    std::filesystem::path root_path = config["export_root"];
    std::filesystem::path log_path = root_path / "log.txt";
    std::ofstream logfile(log_path.c_str(), std::ios::out);

    return 0;

}
