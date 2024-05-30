# Efficient Generation of Multimodal Fluid Simulation Data


## Setup

To download our fork of FluidX3D and a json processing library to the `ext/` folder, run:
```
git submodule update --init --recursive --remote
```
At this point, you should be able to compile our C++ library and executables using CMake on your preferred platform; we recommend you have an OpenCL >= 1.2 distribution on a discoverable path, but one is also packaged with the FluidX3D fork, so that if the automatic search were to fail, that one should be located.

We require a Python installation to run our scripts. The following command installs all required package for our core scripts:

```
pip install numpy tqdm opencv-python
```

To support optional features (loading msgpack, visualizing exported data), run:

```
pip install msgpack msgpack-numpy vedo zstandard
```

Lastly, you should download our mesh/hdri assets from [here]() and extract them into the `assets/` folder in order to replicate the data we describe in the main manuscript.

___
## Core features

### Simulating a single scene

We package configuration files for the inverse rendering dataset scenes (described in main manuscript) in `./config/scenes/nerf`. The `export.python_exec` attribute in these files should be set to a Python executable with the previously listed dependencies installed. Once you do so, you may run:

```
./bin/SimulateScene[.exe] ./config/scenes/<scene-to-simulate>.json
```

The output will be located at the path specified by the `export.out_dir` attribute.

### Generating datasets

Specifications for the `dam-break` and `obstacles` datasets are located at `config/dataset/`. To generate the data, you may run:

```
python script/generate_dataset.py config/dataset/<dataset-to-generate>.json
```

The `export.python_exec` attribute in these files should be set to a Python executable with the previously listed dependencies installed. The `export_root` attribute in the dataset json files specifies the output location. WARNING: these datasets require ~30GB of disk memory, so make sure the destination drive can account for it.

___
## Additional features

### Converting generated datasets to MSGPack

The `script/convert_msgpack.py` script compresses a generated dataset into the efficient MSGPack format, often used in previous research on neural fluid simulation. The dataset **must** be generated using the `particles` output format for this to work. It requires the root folder of the generated dataset as its only positional argument. The output will be located under in the `msgpack` subfolder of the root. The optional parameters are:

* `-f/--fps`: time resolution of the simulations in the dataset to compress
* `-b/--box_face_pts`: how many points to sample from each face of the enclosing boundary
* `-p/--pts_per_boundary`: how many points to sample from other boundaries


### Dataset cleanup

The `script/find_broken.py` performs cleanup of a generated dataset in `particles` format. When the LBM simulation diverges, all the fluid will be moved to a single lattice cell. Thus, this script performs grid clustering on the particles, reporting in the output all simulations for which some frame has a grid cell with "too many" particles. The script requires the root folder of the generated dataset as its only positional argument. The optional parameters are:

* `-d/--delete`: enables broken simulation deletion (will ask for confirmation)
* `-g/--grid_resolution`: number of grid cells in all dimensions for clustering
* `-t/--threshold`: maximum number of particles in a single grid cell
* `-s/--domain_world_size`: width of the domain in all dimensions

### Loading binary geometry format

`script/load_phi.py` defines two functions, `load_phi` and `load_particles`. Both require as input the path to an exported geometry frame from some simulation, either in density or particles format, respectively. The data will be loaded as a numpy array in both cases, but the density will have shape `N x N x N` (where `N` is the simulation lattice resolution) while the particles will have shape `P x 3` (where `P` is the number of simulated particles, specified at simulation time). The script can also be invoked to visualize a sequence of frames, by passing as a positional argument the output folder of some simulation and the following optional parameters:

* `-f/--fps`: time resolution of the simulations in the simulation to visualize
* `-t/--fluid_type`: `particles` or `density`, depending on the export format of the given simulation


___
## Specifications

Here we document how to define individual scenes and datasets. While we use yaml notation for brevity, the actual configuration files should be json.


### Scenes
```yaml
scene_class: "NeRFScene"
sim_params:
    Nx: 256
    Ny: 256
    Nz: 256
    fx: 0.0
    fy: 0.0
    fz: -0.0002
    alpha: 0.0
    beta: 0.0
    P_n: 10000
    P_rho: 1.0
    sigma: 0.0001
    nu: 0.0005
graphics:
    matte_fluid: false
    skybox: .\\assets\\hdri\\pine_attic.png
export:
    geometry: density
    out_dir: .\\export\\nerf-dataset\\ball
    python_exec: C:\\Users\\pc\\miniconda3\\envs\\nerfpic\\python.exe
    make_video_script: .\\script\\make_video.py
    cameras: 
        width: 1080
        height: 1080
        fov: 20.0
        positions: [
            [ 124.83288678,-331.81273504,  19.55696949],
            [ 270.65258085, 243.20538312,  -5.30394423],
            [ -91.8955154 ,-311.08510257,  77.51708739],
            [-373.59328006, -55.66049398, -58.81929096],
            [ 114.41983959, 174.66551027, 194.2670629 ],
            [ 202.80417237,-163.03852772, 154.39140594],
            [-116.66046666,-317.79705088,  53.24946888],
            [ 261.88005629,-189.12782471,  79.61864569],
            [-287.34387137,-168.49080936,  63.05063922],
            [-129.12999863, 357.98791222, -76.75453039]
        ]
        rotations: [
            [380.61704635,157.40200962],
            [131.94251004,161.36604182],
            [343.5426942 ,147.6424733 ],
            [278.47398654,169.62103139],
            [146.77200361,122.94034874],
            [411.20346861,132.65945671],
            [339.84226363,151.83555203],
            [414.1634645 ,147.27049747],
            [300.38622655,150.16343128],
            [199.83490628,172.33089874]
        ]
        zoom: [
            1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0
        ]
    }
    fps: 60
    simulation_steps: 90
    update_dt: 30
    video_seconds: 
obstacles: []
fluids: [
    {
        type: sphere,
        center: [128.0, 128.0, 30.0],
        radius: 15.0
    }
]
```

### Datasets

```yaml
export_root: D:\\data\\FSD\\export\\obstacles 
num_scenes: 400
seed: 188881
constants:
    scene_class: DatasetScene,
    sim_params: 
        Nx: 256
        Ny: 256
        Nz: 256
        fx: 0.0
        fy: 0.0
        fz: -0.0001
        alpha: 0.0
        beta: 0.0
        P_n: 3000
        P_rho: 1.0
        sigma: 0.00003
        nu: 0.002
    graphics: 
        skybox: .\\assets\\hdri\\netball_court.png
    export: 
        geometry: particles
        python_exec: C:\\Users\\pc\\miniconda3\\envs\\nerfpic\\python.exe
        make_video_script: .\\script\\make_video.py
        cameras: 
            width: 1080
            height: 720
            fov: 100.0
            positions: [
                [-42.09743717, 45.51026209,-95.49428007]
            ]
            rotations: [
                [222.76912712,152.33070927]
            ]
            zoom: [
                250.0
            ]
        fps: 50
        simulation_steps: 250
        update_dt: 20
        video_seconds: 5.0
variables: 
    fluids: [
        {
            type: sphere
            N: [256, 256, 256]
            center_range: [
                [128, 128], [128, 128], [153, 240]
            ]
            rad_range: [20, 40]
            on_ground: false
            inside: true
            nsamples: 100
        }
    ],
    obstacles: [
        {
            type: mesh
            N: [256, 256, 256]
            assets: [
                .\\assets\\stl\\baseball.stl,
                .\\assets\\stl\\cube.stl,
                .\\assets\\stl\\bust.stl,
                .\\assets\\stl\\duck.stl,
                .\\assets\\stl\\ship.stl
            ]
            center_range: [
                [128, 128], [128, 128], [45, 60]
            ]
            rot_range: [
                [90.0, 90.0], [0.0, 0.0], [0.0, 90.0]
            ]
            size_range: [70, 120]
            inside: true
            nsamples: 1000
        }
    ]
```