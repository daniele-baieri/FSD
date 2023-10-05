#include "scenes.hpp"
#include <utils/shapes.hpp>



void Droplets::enable_features() {
    fx3d::Settings::EnableFeature(fx3d::Feature::FORCE_FIELD);
    NeRFScene::enable_features();
}

void Droplets::custom_grid_initialization() {
    DatasetScene::custom_grid_initialization();
    uint Nx = lbm->get_Nx(), Ny = lbm->get_Ny(), Nz = lbm->get_Nz();
    uint x, y, z; 
    for(ulong n=0ull; n<lbm->get_N(); n++) { 
        lbm->coordinates(n, x, y, z);
		if(sphere(x, y, z, sphere_fluid[0].center, sphere_fluid[0].radius)) {
			lbm->u.x[n] = vsphere_1.x;  
			lbm->u.y[n] = vsphere_1.y;  
			lbm->u.z[n] = vsphere_1.z;  
		} else if(sphere(x, y, z, sphere_fluid[1].center, sphere_fluid[1].radius)) {
			lbm->u.x[n] = vsphere_2.x;  
			lbm->u.y[n] = vsphere_2.y;  
			lbm->u.z[n] = vsphere_2.z;  
		}
		lbm->F.x[n] = force_field_scale.x*lbm->relative_position(n).x;  
		lbm->F.y[n] = force_field_scale.y*lbm->relative_position(n).y;  
		lbm->F.z[n] = force_field_scale.z*lbm->relative_position(n).z;  
	}
}

void Droplets::config_sim_params(const nlohmann::json &config) {
    if (config.contains("sim_params")) {
        auto params = config["sim_params"];
        std::vector<float> F_scale_ = params["F_scale"];
        std::vector<float> vsphere_1_ = params["vsphere_1"];
        std::vector<float> vsphere_2_ = params["vsphere_2"];
        force_field_scale = float3(F_scale_[0], F_scale_[1], F_scale_[2]);
        vsphere_1 = float3(vsphere_1_[0], vsphere_1_[1], vsphere_1_[2]);
        vsphere_2 = float3(vsphere_2_[0], vsphere_2_[1], vsphere_2_[2]);
    }
    DatasetScene::config_sim_params(config);
}


Droplets::~Droplets() {
    DatasetScene::~DatasetScene();
}