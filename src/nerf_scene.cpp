#include "scenes.hpp"
#include <iostream>
#include <vector>
#include <nlohmann/json.hpp>


void NeRFScene::postprocess() {

    uint Nx = lbm->get_Nx(), Ny = lbm->get_Ny(), Nz = lbm->get_Nz();
    for(ulong n=0ull; n<lbm->get_N(); n++) {  // remove fluid so we can render background
        lbm->flags[n] &= ~TYPE_F;
        lbm->phi[n] = 0.0f;
    }
    lbm->flags.write_to_device();
    lbm->phi.write_to_device();
    
    Camera& main_cam = fx3d::GraphicsSettings::GetCamera();
    float w = (float)main_cam.width;
    float h = (float)main_cam.height;
    std::vector<std::vector<float>> K = {  // row major
        {main_cam.fov / w, 0.0,              w / 2.0f},
        {0.0,              main_cam.fov / h, h / 2.0f},
        {0.0,              0.0,              1.0     }
    }; 
    std::vector<std::vector<float>> Rt;
    float3 Nf((float)Nx / 2.0f, (float)Ny / 2.0f, (float)Nz / 2.0f);  // normalization factor
    
    nlohmann::json camera_params;
    std::ofstream params_file;
    camera_params["intrinsic"] = K;
    for (uint i = 0; i < n_views; i++) {
        switch_ith_camera(i);
        Rt = {  // column major
            {main_cam.R.xx, main_cam.R.xy, main_cam.R.xz, cam_pos[i].x / Nf.x},
            {main_cam.R.yx, main_cam.R.yy, main_cam.R.yz, cam_pos[i].y / Nf.y}, 
            {main_cam.R.zx, main_cam.R.zy, main_cam.R.zz, cam_pos[i].z / Nf.z}, 
        };

        camera_params["Rt"] = Rt;

        params_file.open(get_ith_out_dir(i) + "/params.json");
        params_file << camera_params.dump(4) << std::endl;
        params_file.close();
        
        render_current_frame(get_ith_out_dir(i), "background");

    }

}


bool NeRFScene::is_boundary(uint x, uint y, uint z) const {
    return DatasetScene::is_boundary(x, y, z);
    // return z <= 2 && x >= 70 && x <= 186 && y >= 70 && y <= 186;            // ball
    // return z <= 2 && x >= 80 && x <= 176 && y >= 80 && y <= 176;            // duck
    // return z <= 2 || x == 255 || x == 0 || y == 255 || y == 0 || z == 255;     // ship, dam
    // return false;                                                              // droplets
}