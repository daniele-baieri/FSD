#pragma once

#include <filesystem>
#include <vector>
#include <fx3d/scenes.hpp>


using float2 = std::pair<float, float>;


enum ExportType {
    PARTICLES,
    DENSITY
};



class DatasetScene : public fx3d::Scene {

protected:

    ExportType            geometry_exp;
    std::filesystem::path out_dir_path;
    std::string           python;
    std::string           script_png2mp4;
    std::vector<float3>   cam_pos;
    std::vector<float2>   cam_rot;
    std::vector<float>    cam_zoom;
    uint                  n_views;

    float3 v_init;

    /* Video output */
    void switch_ith_camera(const uint i) const;
    void make_ith_video(const uint i) const;
    std::string get_ith_out_dir(const uint i) const;

    /* Geometry output */
    std::string geometry_out_dir() const;
    std::string boundary_out_dir() const;
    void export_geometry() const;
    void export_boundary_meshes() const;

    virtual void config_sim_params(const nlohmann::json &config);
    virtual void config_units(const nlohmann::json &config);
    virtual void config_export(const nlohmann::json &config) override;
    virtual void select_rendering_mode(const nlohmann::json &config);

    virtual void custom_grid_initialization();

    virtual void postprocess() override;
    
    virtual void enable_features();

public:

    virtual void export_frame() override;

	DatasetScene() = default;
    ~DatasetScene(); 

};


class Ballistic : public DatasetScene {

public:

    virtual bool is_boundary(uint x, uint y, uint z) const;

	Ballistic() = default;
    ~Ballistic(); 

};


class NeRFScene : public DatasetScene {


protected:

    void postprocess() override;

public:

    void enable_features() override {
        fx3d::Settings::EnableFeature(fx3d::Feature::PARTICLES);
        fx3d::Scene::enable_features();
    }
    bool is_boundary(uint x, uint y, uint z) const;


};


class Droplets : public NeRFScene {

private:

    float3 force_field_scale;
    float3 vsphere_1;
    float3 vsphere_2;
    
protected:

    virtual void enable_features();
    virtual void config_units(const nlohmann::json &config);
    virtual void config_sim_params(const nlohmann::json &config);

public:

    virtual void custom_grid_initialization();

	Droplets() = default;
    ~Droplets(); 

};
