#include "scenes.hpp"
#include <utils/shapes.hpp>


void Ballistic::config_fluid_bodies(const nlohmann::json &config) {
	if (config.contains("fluids")) {
		nlohmann::json fluids = config["fluids"];
        auto s = fluids[0];
        std::vector<float> c1 = s["center"];
        radius = s["radius"];
        center = float3(c1[0], c1[1], c1[2]);
    }
}

void Ballistic::custom_grid_initialization() {
    uint Nx = lbm->get_Nx(), Ny = lbm->get_Ny(), Nz = lbm->get_Nz();
    uint x, y, z; 
    for(ulong n=0ull; n<lbm->get_N(); n++) { 
        lbm->coordinates(n, x, y, z);
		if(sphere(x, y, z, center, radius)) {
			lbm->flags[n] = TYPE_F;
			lbm->u.x[n] = v_init.x;
            lbm->u.y[n] = v_init.y;
            lbm->u.z[n] = v_init.z;
		}
		if(x==0u||x==Nx-1u||y==0u||y==Ny-1u||z<=2u||z==Nz-1u) 
            lbm->flags[n] = TYPE_S; // all non periodic
	}
}

void Ballistic::config_sim_params(const nlohmann::json &config) {
    if (config.contains("sim_params")) {
        auto params = config["sim_params"];
        std::vector<float> v_init_ = params["v_init"];
        v_init = float3(v_init_[0], v_init_[1], v_init_[2]);
    }
    DatasetScene::config_sim_params(config);
}

Ballistic::~Ballistic() {
    DatasetScene::~DatasetScene();
}