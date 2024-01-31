#include "scenes.hpp"
#include <iostream>
#include <utils/coo_tensor_float.hpp>
#include <fx3d/settings.hpp>


// void FSDScene::config_graphics(const nlohmann::json &config) {
//    lbm->graphics.visualization_modes = VIS_PHI_RAYTRACE;
// }

void DatasetScene::config_export(const nlohmann::json &config) {
	if (config.contains("export")) {
		nlohmann::json exp_config = config["export"];
		python = exp_config["python_exec"];
        script_png2mp4 = exp_config["make_video_script"];

        if (exp_config.contains("geometry")) {
            std::string geometry = exp_config["geometry"];
            if (geometry == "particles") {
                geometry_exp = ExportType::PARTICLES;
            } else if (geometry == "density") {
                geometry_exp = ExportType::DENSITY;
            } else {
                std::cerr << "Unknown geometry type, defaulting to ExportType::DENSITY" << std::endl;
            }
        } else {
            std::cerr << "Geometry export undefined, defaulting to ExportType::DENSITY" << std::endl;
            geometry_exp = ExportType::DENSITY;
        }

        if (exp_config.contains("cameras")) {
            nlohmann::json cam_config = exp_config["cameras"];
            fx3d::GraphicsSettings::SetWidth(cam_config["width"]);
            fx3d::GraphicsSettings::SetHeight(cam_config["height"]);
            Camera& cam = fx3d::GraphicsSettings::GetCamera();
            delete[] cam.zbuffer;
            delete[] cam.bitmap;
            cam.free = true;
            cam.bitmap = new int[cam.width * cam.height];
            cam.zbuffer = new int[cam.width * cam.height];
            cam.set_zoom(1.0);
            cam.fov = cam_config["fov"];
            cam.update_matrix();
            auto positions = cam_config["positions"];
            auto rotations = cam_config["rotations"];
            auto zooms = cam_config["zoom"];
            assert(positions.size() == rotations.size());
            assert(positions.size() == zooms.size());
            n_views = positions.size();
            for (uint i = 0; i < n_views; i++) {
                std::vector<float> xyz = positions[i];
                std::vector<float> uv = rotations[i];
                float z = zooms[i];
                cam_pos.emplace_back(xyz[0], xyz[1], xyz[2]);
                cam_rot.emplace_back(radians(uv[0]), radians(uv[1]));
                cam_zoom.push_back(z);
            }
        }
	}

    fx3d::Scene::config_export(config);
    if (config.contains("export")) {
        const float dt = units.si_t(1.0);
        const uint steps_per_sec = ceil(1.0 / dt);
        update_dt = ceil(float(steps_per_sec) / float(fps));
        std::cout << "lbm steps per output frame = " << update_dt << std::endl;
    }

    out_dir_path = get_out_dir();
    
    std::string geometry_out = geometry_out_dir();
	if (!std::filesystem::exists(geometry_out)) {
		std::filesystem::create_directories(geometry_out);
    }

    std::string boundary_out = boundary_out_dir();
	if (!std::filesystem::exists(boundary_out)) {
		std::filesystem::create_directories(boundary_out);
    }
}

void DatasetScene::config_sim_params(const nlohmann::json &config) {
    if (config.contains("sim_params")) {
        auto params = config["sim_params"];
        if (params.contains("v_init")) {
            std::vector<float> v_init_ = params["v_init"];
            v_init = float3(v_init_[0], v_init_[1], v_init_[2]);
        }
    }
    fx3d::Scene::config_sim_params(config);
}

void DatasetScene::config_units(const nlohmann::json &config) {
    fx3d::Scene::config_units(config);
    const float lbm_d = max(Nx, max(Ny, Nz));
	// const float si_nu = 1E-6f; // kinematic shear viscosity (water) [m^2/s]
	const float si_rho = 1E3f; // density (water) [kg/m^3]
	const float si_sigma = 0.072f; // surface tension (water) [kg/s^2]
    const float si_d = 2.0f; // Size of the domain [m]
	const float si_g = 9.81f; // gravitational acceleration [m/s^2]
	const float si_f = units.si_f_from_si_g(si_g, si_rho);
	const float lbm_rho = 1.0f;  // avg density always 1 in LBM
	const float m = si_d/lbm_d; // length si_x = x*[m]
	const float kg = si_rho/lbm_rho*cb(m); // density si_rho = rho*[kg/m^3]
	const float s = sqrt(sigma/si_sigma*kg); // velocity si_sigma = sigma*[kg/s^2]
	units.set_m_kg_s(m, kg, s); // do unit conversion manually via d, rho and sigma
    std::cout << "f = " << -units.f(si_f) << std::endl;
    std::cout << "nu = " << units.nu(nu) << std::endl;
    std::cout << "m = " << units.si_x(1.0f) << std::endl;
    std::cout << "kg = " << units.si_M(1.0f) << std::endl;
    std::cout << "s = " << units.si_t(1.0f) << std::endl;
    std::cout << "sigma = " << units.si_sigma(sigma) << std::endl;

    sigma = units.sigma(si_sigma);
    fz = -units.f(si_f);
    nu = units.nu(nu);
}

void DatasetScene::custom_grid_initialization() {
    fx3d::Scene::custom_grid_initialization();
    uint Nx = lbm->get_Nx(), Ny = lbm->get_Ny(), Nz = lbm->get_Nz();
    uint x, y, z; 
    for(ulong n=0ull; n<lbm->get_N(); n++) { 
        lbm->coordinates(n, x, y, z);
		if (is_fluid(x, y, z)) {
			lbm->u.x[n] = units.u(v_init.x);
            lbm->u.y[n] = units.u(v_init.y);
            lbm->u.z[n] = units.u(v_init.z);
		}
	}
}




void DatasetScene::switch_ith_camera(const uint i) const {
    Camera& cam = fx3d::GraphicsSettings::GetCamera();
    cam.pos = cam_pos[i];
    cam.rx = cam_rot[i].first;
    cam.ry = cam_rot[i].second;
    cam.set_zoom(cam_zoom[i]);
    cam.update_matrix();
}

std::string DatasetScene::get_ith_out_dir(const uint i) const {
    return (out_dir_path / ("camera_" + std::to_string(i))).string();
}

void DatasetScene::make_ith_video(const uint i) const {
    std::string view_out = get_ith_out_dir(i);
    std::string cmd = python + " " + script_png2mp4 + " " + view_out;
    system(cmd.c_str());
}

std::string DatasetScene::geometry_out_dir() const {
    return (out_dir_path / "frames").string();
}

std::string DatasetScene::boundary_out_dir() const {
    return (out_dir_path / "boundary_mesh").string();
}

void DatasetScene::export_geometry() const {
    std::string geometry_out = geometry_out_dir();
    if (geometry_exp == ExportType::DENSITY) {
        lbm->phi.write_device_to_sparse(geometry_out);
    } else if (geometry_exp == ExportType::PARTICLES) {
        lbm->write_particles(fx3d::default_filename(geometry_out, "particles", ".dat", lbm->get_t()));
    }
}

void DatasetScene::export_boundary_meshes() const {
    std::string boundary_out = boundary_out_dir();
    std::string fname;
    for (uint i = 0; i < mesh_obst.size(); i++) {
        fname = fx3d::default_filename(boundary_out, "mesh", ".stl", i);
        write_stl(fname, mesh_obst[i]);
    }
}

void DatasetScene::export_frame() {
    std::string view_out;
    for (uint i = 0; i < n_views; i++) {
        view_out = get_ith_out_dir(i);
        switch_ith_camera(i);
        render_current_frame(view_out, "frame");
    }
    export_geometry();
}

void DatasetScene::postprocess() {
    for (uint i = 0; i < n_views; i++) {
        make_ith_video(i);
    }
    export_boundary_meshes();
}

void DatasetScene::enable_features() {
    fx3d::Settings::EnableFeature(fx3d::Feature::PARTICLES);
    fx3d::Scene::enable_features();
}

DatasetScene::~DatasetScene() {
    fx3d::Scene::~Scene();
}


void DatasetScene::select_rendering_mode(const nlohmann::json &config) {
    lbm->graphics.visualization_modes = VIS_PHI_RAYTRACE;
}