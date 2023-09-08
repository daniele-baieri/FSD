#include <iostream>
#include <string>
#include <filesystem>
#include <stdlib.h>
#include <fx3d/lbm.hpp>
#include <fx3d/settings.hpp>
#include <fx3d/scenes.hpp>
#include <fx3d/info.hpp>
#include "dataset_scene.hpp"




fx3d::Info fx3d::info;
Units units;


void make_scenes(const nlohmann::json &config) {

}

void generate(const std::filesystem::path& root, const std::ofstream& log) {

}



int main(int argc, char** argv) {

    std::ifstream stream(argv[1]);
    nlohmann::json config = nlohmann::json::parse(stream);

    std::cout << "Generating scene configuration files..." << std::endl;

    make_scenes(config);


    std::filesystem::path root_path = config["export_root"];
    std::filesystem::path log_path = root_path / "log.txt";
    std::ofstream logfile(log_path.c_str(), std::ios::out);

    std::cout << "Simulating generated scenes..." << std::endl;

    generate(root_path, logfile);

    return 0;

}
