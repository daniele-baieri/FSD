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
scene_class: # "Scene". See below for description.
sim_params:
    Nx:    # Lattice units in x dimension
    Ny:    # Lattice units in y dimension
    Nz:    # Lattice units in z dimension
    fx:    # Global volume force in x dimension [LBM units]
    fy:    # Global volume force in y dimension [LBM units]
    fz:    # Global volume force in z dimension [LBM units]
    alpha: # Thermal diffusion coefficient (average temperature is assumed 1.0)
    beta:  # Thermal expansion coefficient (average temperature is assumed 1.0)
    P_n:   # Number of particles to simulate (can be zero)
    P_rho: # Particle density (generally leave to 1.0)
    sigma: # Surface tension [LBM units]
    nu:    # Viscosity [LBM units]
graphics:
    matte_fluid: # Fluid appearence. true for matte, false for water
    skybox:      # Path to a .png HDRI map
export:
    out_dir:           # Output path for simulation
    python_exec:       # Path to Python interpreter for external scripts invocation
    make_video_script: # Script to convert frame renders to videos, generally leave as "./scripts/make_video.py"
    fps:               # Frequency of exported frames
    simulation_steps:  # Total number of output frames
    update_dt:         # Number of LBM steps for an output frame
    video_seconds:     # Total simulation time [s]
obstacles: # A list of specifications for boundary geometry. See below.
fluids:    # A list of specifications for the initial state of all fluid bodies. See below.
```

#### Scene classes

The **Scene** class allows for full customization: simulation parameters are in LBM units and there are no constraints on the specification. For convenience, we define more specific scene types which also support additional features.

The **DatasetScene** class accepts simulation parameters in real world units (see main manuscript) and allows for vast customization of the export. In particular, it allows to export geometry and define multiple rendering cameras. The updated class specification is:
```yaml
scene_class: # "DatasetScene"
sim_params:
    fx:     # Global volume force in x dimension [m/s^2]
    fy:     # Global volume force in y dimension [m/s^2]
    fz:     DEPRECATED  # Fixed to -9.81 [m/s^2] (specified value is ignored)
    nu:     # Kinematic shear viscosity [m^2/s]
    v_init: # 3D initial velocity of the fluid [m/s]
export:
    geometry:  # "density" or "particles"
    update_dt: DEPRECATED  # Specified value is ignored, actual value is computed.
    cameras: 
        width:     # Rendering width
        height:    # Rendering height
        fov:       # Field of view of rendering camera
        positions: # A list of points specifying multiple camera positions.
                   # Coordinates have no specific ranges, but the scene is embedded in [[0; Nx], [0; Ny], [0; Nz]]
        rotations: # A list of camera rotations, as [x rotation, z rotation] in degrees.
                   # We recommend generating these with the "./scripts/generate_nerf_cameras.py" script.
                   # Must have the same number of elements as export.cameras.positions.
        zoom:      # A list of zoom values for the specified cameras.
                   # Must have the same number of elements as export.cameras.positions.
```

The **NeRFScene** class has the same configuration of a **DatasetScene**, but it implements exporting camera parameters for all viewpoints. The only required variation is
```yaml
scene_class: # "NeRFScene"
```

#### Obstacles and fluid specifications

To add geometry to the scene, you should specify one or more geometric primitives in the `obstacles` and `fluids` attributes. Three types of primitives are currently supported. We show the syntax template below. Recall that all coordinates have no specific range, but the scene is embedded in `[[0; Nx], [0; Ny], [0; Nz]]`. 
```yaml
{
    type: sphere
    center: # Center of the sphere in the simulation domain 
    radius: # Radius of the sphere
},
{
    type: cuboid
    center:   # Center of the sphere in the simulation domain
    rotation: # Rotation of the cuboid over all three axes, in degrees
    sides:    # Length of cuboid sides in all three axes
},
{
    type: mesh
    mesh_path: # Path to an STL mesh to load
    center:    # Center of the mesh in the simulation domain. 
    rotation:  # Rotation of the object over all three axes, in degrees
    scale:     # Scaling of the object over all three axes
    size:      # Scale the shape by specifying the maximum bounding box side length
               # (Does not change shape proportions)
}

```


### Datasets

To specify a dataset for generation, three types of settings are required: a) global generation parameters, b) scene constants, and c) scene variables. We describe the three categories separately.

#### Global generation parameters

```yaml
export_root: # Path to the directory that will contain the generated dataset
num_scenes:  # How many scenes should the dataset contain
seed:        # Random seed for dataset replication
```

#### Scene constants

Under the
```yaml
constants:
```
attribute, users may specify Scene attributes identically to what we showed in the previous section. These will be identical for all scenes in the dataset: for instance, it makes sense to have constant simulation resolution throughout the dataset. For example:

```yaml
constants:
    sim_params:
        Nx: 128
        Ny: 128
        Nz: 128
```

#### Scene variables

We define a number of samplers for Scene variables. Under the 
```yaml
variables: 
```
attribute, following the structure specified for Scenes in the previous section, users may specify a sampler configuration rather than a value for the dataset variables. 
The set of all possible scenes is defined by the cartesian product of all sample sets of each variable. Then, a uniformly random subset of all these is chosen, with its cardinality specified via the `num_scenes` global attribute.
We show the syntax for all our sampler types. First, we specify primitive distributions for float variables:

```yaml
{
    type: collection
    values: # A list of float values
},
{
    type: linscale
    vmin:   # (float) Minimum value for linear space
    vmax:   # (float) Maximum value for linear space
    nsteps: # Number of steps for linear space
},
{
    type: normal
    mean:     # (float) Mean for the normal distribution
    std:      # (float) std for the normal distribution
    nsamples: # Number of samples from normal distribution
},
{
    type: uniform  # Uniform distribution over a float range
    lo:       # (float) Lower bound of uniform distribution
    hi:       # (float) Upper bound of uniform distribution
    nsamples: # Number of samples from uniform distribution
}
```

Then, we also support random distributions for complex types such as geometric structures, to add variation to the datasets also in terms of initial fluid state and boundary geometry.

```yaml
{
    type: sphere
    N:            # Shortcut for simulation resolution [Nx, Ny, Nz]
    center_range: # Ranges to sample the center coordinates uniformly
                  # [[x_min, x_max], [y_min, y_max], [z_min, z_max]]
    rad_range:    # Range to sample the radius uniformly [r_min, r_max]
    on_ground:    # Translate the sampled sphere to intersect the z=0 plane
    inside:       # Translate the sampled sphere to ensure it is completely inside the simulation domain
    nsamples:     # How many spheres to sample
},
{
    type: cuboid,
    N:            # Shortcut for simulation resolution [Nx, Ny, Nz]
    center_range: # Ranges to sample the center coordinates uniformly
                  # [[x_min, x_max], [y_min, y_max], [z_min, z_max]]
    sides_range:  # Ranges to sample the cuboid sides uniformly
                  # [[sx_min, sx_max], [sy_min, sy_max], [sz_min, sz_max]]
    rot_range:    # Ranges to sample the cuboid rotation (in degrees) uniformly
                  # [[rx_min, rx_max], [ry_min, ry_max], [rz_min, rz_max]]
    on_ground:    # Translate the sampled cuboid to intersect the z=0 plane
    nsamples:     # How many cuboids to sample
},
{
    type: mesh,
    N:            # Shortcut for simulation resolution [Nx, Ny, Nz]
    assets:       # A list of STL meshes to sample the geometry from
    center_range: # Ranges to sample the center coordinates uniformly
                  # [[x_min, x_max], [y_min, y_max], [z_min, z_max]]
    rot_range:    # Ranges to sample the object rotation (in degrees) uniformly
                  # [[rx_min, rx_max], [ry_min, ry_max], [rz_min, rz_max]]
    size_range:   # Range to sample the object size uniformly [s_min, s_max]
    inside:       # Translate the sampled object to ensure it is completely inside the simulation domain
    nsamples:     # How many objects to sample
}
```

An example:

```yaml
variables:
    sim_params:
        nu: 
            type: linscale
            vmin: 0.001
            vmax: 0.004
            nsteps: 100
    graphics:
        skybox:
            type: collection
            values:
                - ".\\assets\\hdri\\netball_court.png"
                - ".\\assets\\hdri\\pine_attic.png"
    obstacles:
        - type: mesh,
          N: [256, 256, 256],
          assets: [
              .\\assets\\stl\\baseball.stl,
              .\\assets\\stl\\cube.stl,
              .\\assets\\stl\\bust.stl,
              .\\assets\\stl\\duck.stl,
              .\\assets\\stl\\ship.stl
          ],
          center_range: [
              [128, 128], [128, 128], [45, 60]
          ],
          rot_range: [
              [90.0, 90.0], [0.0, 0.0], [0.0, 90.0]
          ],
          size_range: [70, 120],
          inside: true,
          nsamples: 1000
