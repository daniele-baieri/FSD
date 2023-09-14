#include "scenes.hpp"
#include <utils/shapes.hpp>


void Droplets::config_fluid_bodies(const nlohmann::json &config) {
	if (config.contains("fluids")) {
		nlohmann::json fluids = config["fluids"];
        auto s1 = fluids[0];
        std::vector<float> c1 = s1["center"];
        s1_radius = s1["radius"];
        s1_center = float3(c1[0], c1[1], c1[2]);
        auto s2 = fluids[1];
        std::vector<float> c2 = s2["center"];
        s2_radius = s2["radius"];
        s2_center = float3(c2[0], c2[1], c2[2]);
    }
}

void Droplets::enable_features() {
    fx3d::Settings::EnableFeature(fx3d::Feature::FORCE_FIELD);
    DatasetScene::enable_features();
}

void Droplets::custom_grid_initialization() {
    uint Nx = lbm->get_Nx(), Ny = lbm->get_Ny(), Nz = lbm->get_Nz();
    uint x, y, z; 
    for(ulong n=0ull; n<lbm->get_N(); n++) { 
        lbm->coordinates(n, x, y, z);
		if(sphere(x, y, z, s1_center, s1_radius)) {
			lbm->flags[n] = TYPE_F;
			lbm->u.y[n] = v_init.first;  
		}
		if(sphere(x, y, z, s2_center, s2_radius)) {
			lbm->flags[n] = TYPE_F;
			lbm->u.y[n] = v_init.second;  
		}
		lbm->F.x[n] = force_field_scale.x*lbm->relative_position(n).x;  
		lbm->F.y[n] = force_field_scale.y*lbm->relative_position(n).y;  
		lbm->F.z[n] = force_field_scale.z*lbm->relative_position(n).z;  
		if(x==0u||x==Nx-1u||y==0u||y==Ny-1u||z==0u||z==Nz-1u) 
            lbm->flags[n] = TYPE_S; 
	}
}

void Droplets::config_sim_params(const nlohmann::json &config) {
    if (config.contains("sim_params")) {
        auto params = config["sim_params"];
        std::vector<float> F_scale_ = params["F_scale"];
        std::vector<float> v_init_ = params["v_init"];
        force_field_scale = float3(F_scale_[0], F_scale_[1], F_scale_[2]);
        v_init = float2(v_init_[0], v_init_[1]);
    }
    DatasetScene::config_sim_params(config);
}


Droplets::~Droplets() {
    DatasetScene::~DatasetScene();
}