#pragma once

#include <filesystem>
#include <vector>
#include <fx3d/scenes.hpp>


class DatasetScene : public fx3d::Scene {

private:

    std::filesystem::path out_dir_path;
    std::string python;
    std::string script_png2mp4;
    std::vector<float3> cam_pos;
    std::vector<std::pair<float, float>> cam_rot;
    std::vector<float> cam_zoom;
    uint n_views;

protected:

    /* Video output */
    void switch_ith_camera(const uint i) const;
    void make_ith_video(const uint i) const;
    std::string get_ith_out_dir(const uint i) const;

    /* Geometry output */
    std::string geometry_out_dir() const;
    void export_geometry() const;

    // virtual void config_graphics(const nlohmann::json &config) override; 
    virtual void config_export(const nlohmann::json &config) override;

    virtual void postprocess() override;

public:

    virtual void export_frame() override;

	DatasetScene() = default;
    ~DatasetScene(); 

};