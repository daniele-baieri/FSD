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

    ExportType                           geometry_exp;
    std::filesystem::path                out_dir_path;
    std::string                          python;
    std::string                          script_png2mp4;
    std::vector<float3>                  cam_pos;
    std::vector<std::pair<float, float>> cam_rot;
    std::vector<float>                   cam_zoom;
    uint                                 n_views;

    /* Video output */
    void switch_ith_camera(const uint i) const;
    void make_ith_video(const uint i) const;
    std::string get_ith_out_dir(const uint i) const;

    /* Geometry output */
    std::string geometry_out_dir() const;
    void export_geometry() const;

    // virtual void config_graphics(const nlohmann::json &config) override; 
    virtual void config_export(const nlohmann::json &config) override;
    virtual void select_rendering_mode(const nlohmann::json &config);

    virtual void postprocess() override;
    
    virtual void enable_features();

public:

    virtual void export_frame() override;

	DatasetScene() = default;
    ~DatasetScene(); 

};


class DamBreak : public DatasetScene {

private:

    float3   dam_center;
    float3   dam_sides;
    float3x3 dam_rotate;

protected:

    virtual void config_fluid_bodies(const nlohmann::json &config);

public:

    virtual bool is_fluid(uint x, uint y, uint z) const;

	DamBreak() = default;
    ~DamBreak(); 

};


class BallDrop : public DatasetScene {

private:

    float3 ball_center;
    float  ball_radius;

protected:

    virtual void config_fluid_bodies(const nlohmann::json &config);

public:

    virtual bool is_fluid(uint x, uint y, uint z) const;

	BallDrop() = default;
    ~BallDrop(); 

};


class Droplets : public DatasetScene {

private:

    float3 s1_center;
    float3 s2_center;
    float  s1_radius;
    float  s2_radius;
    float3 force_field_scale;
    float2 v_init;
    
protected:

    virtual void enable_features();
    virtual void config_fluid_bodies(const nlohmann::json &config);
    virtual void config_sim_params(const nlohmann::json &config);

public:

    virtual void custom_grid_initialization();

	Droplets() = default;
    ~Droplets(); 

};


class Ballistic : public DatasetScene {

private:

    float3 center;
    float  radius;
    float3 v_init;


protected:

    virtual void config_fluid_bodies(const nlohmann::json &config);
    virtual void config_sim_params(const nlohmann::json &config);

public:

    virtual bool is_fluid(uint x, uint y, uint z) const;
    virtual bool is_boundary(uint x, uint y, uint z) const;
    virtual void custom_grid_initialization();

	Ballistic() = default;
    ~Ballistic(); 

};


class NeRFScene : public DatasetScene {


protected:

    void postprocess() override;

public:


    bool is_boundary(uint x, uint y, uint z) const;


};