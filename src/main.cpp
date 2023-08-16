#include <iostream>
#include <string>
#include <stdlib.h>
#include <fx3d/lbm.hpp>
#include <fx3d/settings.hpp>
#include <fx3d/scenes.hpp>
#include <fx3d/info.hpp>
#include "myscene.hpp"




fx3d::Info fx3d::info;



int main(int argc, char** argv) {


	fx3d::info.print_logo();

	std::ifstream stream(argv[1]);
	nlohmann::json j = nlohmann::json::parse(stream);

    fx3d::Settings::EnableFeature(fx3d::Feature::VOLUME_FORCE);
    fx3d::Settings::EnableFeature(fx3d::Feature::SURFACE);
    fx3d::GraphicsSettings::SetWidth(1080);
    fx3d::GraphicsSettings::SetHeight(720);

	TestScene scene;
	scene.configure(j);

	scene.run();

	std::string python = j["export"]["python_exec"];
	std::string png_dir = j["export"]["out_dir"];
	std::string make_video_cmd = python + " ./script/make_video.py " + png_dir;
	system(make_video_cmd.c_str());

	return 0;

}
