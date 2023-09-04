
#include <dataset_scene.hpp>
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
        if (exp_config.contains("cameras")) {
            nlohmann::json cam_config = exp_config["cameras"];
            fx3d::GraphicsSettings::SetWidth(cam_config["width"]);
            fx3d::GraphicsSettings::SetHeight(cam_config["height"]);
            Camera& cam = fx3d::GraphicsSettings::GetCamera();
            delete[] cam.zbuffer;
            delete[] cam.bitmap;
            cam.bitmap = new int[cam.width * cam.height];
            cam.zbuffer = new int[cam.width * cam.height];
            cam.set_zoom(1.0);
            cam.fov = cam_config["fov"];
            cam.update_matrix();
            auto positions = cam_config["positions"];
            auto rotations = cam_config["rotations"];
            assert(positions.size() == rotations.size());
            n_views = positions.size();
            for (auto ptr = positions.begin(); ptr != positions.end(); ptr++) {
                std::vector<float> xyz = *ptr;
                cam_pos.emplace_back(xyz[0], xyz[1], xyz[2]);
            }
            for (auto ptr = rotations.begin(); ptr != rotations.end(); ptr++) {
                std::vector<float> uv = *ptr;
                cam_rot.emplace_back(radians(uv[0]), radians(uv[1]));
            }
        }
	}
    fx3d::Scene::config_export(config);
    out_dir_path = get_out_dir();
    
    std::string geometry_out = geometry_out_dir();
	if (!std::filesystem::exists(geometry_out)) {
		std::filesystem::create_directories(geometry_out);
    }
}

void DatasetScene::switch_ith_camera(const uint i) const {
    Camera& cam = fx3d::GraphicsSettings::GetCamera();
    cam.pos = cam_pos[i];
    cam.rx = cam_rot[i].first;
    cam.ry = cam_rot[i].second;
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
    return (out_dir_path / "density").string();
}

void DatasetScene::export_geometry() const {
    std::string geometry_out = geometry_out_dir();
    lbm->phi.write_device_to_sparse(geometry_out);
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
}

DatasetScene::~DatasetScene() {
    fx3d::Scene::~Scene();
}
