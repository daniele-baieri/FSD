#include "scenes.hpp"
#include <utils/shapes.hpp>


void BallDrop::config_fluid_bodies(const nlohmann::json &config) {
	if (config.contains("fluids")) {
		nlohmann::json fluids = config["fluids"];
        // Ignore entries after the first one
		for (auto ptr = fluids.begin(); ptr != fluids.end(); ptr++) {
			auto fluid = *ptr;
			std::vector<float> center = fluid["center"];
			ball_radius = fluid["radius"];
            ball_center = float3(center[0], center[1], center[2]);
            break;
		}
    }
}


bool BallDrop::is_fluid(uint x, uint y, uint z) const {
    return sphere(x, y, z, ball_center, ball_radius);
}


BallDrop::~BallDrop() {
    DatasetScene::~DatasetScene();
}