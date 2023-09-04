#pragma once

#include <filesystem>
#include <vector>
#include <dataset_scene.hpp>


class DamBreak : public DatasetScene {

private:

    float3 dam_center;
    float3 dam_sides;
    float3x3 dam_rotate;

protected:

    virtual void config_fluid_bodies(const nlohmann::json &config);

public:

    virtual bool is_fluid(uint x, uint y, uint z) const;

	DamBreak() = default;
    ~DamBreak(); 

};