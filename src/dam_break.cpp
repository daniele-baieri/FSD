#include <dam_break.hpp>
#include <utils/shapes.hpp>


void DamBreak::config_fluid_bodies(const nlohmann::json &config) {
	if (config.contains("fluids")) {
		nlohmann::json fluids = config["fluids"];
        // Ignore entries after the first one
		for (auto ptr = fluids.begin(); ptr != fluids.end(); ptr++) {
			auto fluid = *ptr;
			std::vector<float> center = fluid["center"];
			std::vector<float> sides = fluid["sides"];
            std::vector<float> rot = fluid["rotation"];
            dam_center.x = center[0]; dam_center.y = center[1]; dam_center.z = center[2];
            dam_sides.x = sides[0]; dam_sides.y = sides[1]; dam_sides.z = sides[2];
            dam_rotate = euler(float3(rot[0], rot[1], rot[2]));
		}
    }
}


bool DamBreak::is_fluid(uint x, uint y, uint z) const {
    float3 rotated = dam_rotate * float3(x, y, z);
    return cuboid(floor(rotated.x), floor(rotated.y), floor(rotated.z), dam_center, dam_sides);
}


DamBreak::~DamBreak() {
    DatasetScene::~DatasetScene();
}