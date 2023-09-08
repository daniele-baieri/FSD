#include <iostream>
#include <string>
#include <stdlib.h>
#include <fx3d/lbm.hpp>
#include <fx3d/settings.hpp>
#include <fx3d/scenes.hpp>
#include <fx3d/info.hpp>
#include "dataset_scene.hpp"
#include "dam_break.hpp"
#include "scene_select.hpp"




fx3d::Info fx3d::info;
Units units;


int main(int argc, char** argv) {


    fx3d::info.print_logo();

    std::ifstream stream(argv[1]);
    nlohmann::json j = nlohmann::json::parse(stream);

    fx3d::Settings::EnableFeature(fx3d::Feature::VOLUME_FORCE);
    fx3d::Settings::EnableFeature(fx3d::Feature::SURFACE);

    // DatasetScene scene;
    std::string scene_type = j["scene_class"];
    fx3d::Scene* scene = select_scene_type(scene_type);
    
    scene->configure(j);
    scene->run();

    delete scene;
    return 0;

}
