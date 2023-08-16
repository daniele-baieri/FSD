#include <fx3d/scenes.hpp>


class TestScene : public fx3d::Scene {

protected:

    virtual void config_graphics(const nlohmann::json &config) override; 

public:

	TestScene() = default;
    ~TestScene(); 

};