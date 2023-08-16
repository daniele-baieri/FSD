#include "test_scene.hpp"


void TestScene::config_graphics(const nlohmann::json &config) {
    lbm->graphics.visualization_modes = VIS_PHI_RASTERIZE|VIS_FLAG_SURFACE;
}

TestScene::~TestScene() {
    fx3d::Scene::~Scene();
}