#pragma once

#include <map>
#include <string>
#include <fx3d/scenes.hpp>
#include "scenes.hpp"




const std::map<std::string, std::function<fx3d::Scene*()>> SceneTypeMap {
    {"Scene", [](){return new fx3d::Scene();}},
    {"DatasetScene", [](){return new DatasetScene();}},
    {"Droplets", [](){return new Droplets();}},
    {"Ballistic", [](){return new Ballistic();}},
    {"NeRFScene", [](){return new NeRFScene();}}
};


fx3d::Scene* select_scene_type(const std::string& type) {
    auto it = SceneTypeMap.find(type);
    if (it != SceneTypeMap.end()) {
        return (it->second)();
    } else {
        return nullptr;
    }
}