import sys
import json
import copy
import pathlib
import itertools
import subprocess
import numpy as np
from hashlib import shake_256



SIMULATE_EXEC = "./bin/SimulateScene.exe"


class Cuboid:

    def __init__(self, center, sides, rotation):
        self.center = np.floor(center)
        self.sides = np.floor(sides)
        self.rotation = np.floor(rotation)

    def __str__(self):
        return str([self.center, self.sides, self.rotation])

    def to_dict(self):
        return {
            'center': [float(self.center[0]), float(self.center[1]), float(self.center[2])],
            'sides': [float(self.sides[0]), float(self.sides[1]), float(self.sides[2])],
            'rotation': [float(self.rotation[0]), float(self.rotation[1]), float(self.rotation[2])]
        }


class Sphere:

    def __init__(self, center, radius):
        self.center = np.floor(center)
        self.radius = radius

    def __str__(self):
        return str([self.center, self.radius])

    def to_dict(self):
        return {
            'center': [float(self.center[0]), float(self.center[1]), float(self.center[2])],
            'radius': float(self.radius)
        }



def sample_collection(collection):
    return collection

def sample_linscale(vmin, vmax, nsteps):
    out = np.linspace(vmin, vmax, nsteps)
    return out.tolist()

def sample_normal(mean, std, nsamples):
    out = np.random.normal(mean, std, (nsamples,))
    return out.tolist()

def sample_uniform(lo, hi, nsamples):
    out = np.random.uniform(lo, hi, (nsamples,))
    return out.tolist()

def sample_cuboid(N, center_range, sides_range, rot_range, on_ground, nsamples):

    out = []
    cx = np.random.uniform(center_range[0][0], center_range[0][1], (nsamples,))
    cy = np.random.uniform(center_range[1][0], center_range[1][1], (nsamples,))
    if on_ground:
        cz = np.random.uniform(center_range[2][0], min(center_range[2][1], N[2]/2), (nsamples,))
    else:
        cz = np.random.uniform(center_range[2][0], center_range[2][1], (nsamples,))

    rx = np.random.uniform(rot_range[0][0], rot_range[0][1], (nsamples,))
    ry = np.random.uniform(rot_range[1][0], rot_range[1][1], (nsamples,))
    rz = np.random.uniform(rot_range[2][0], rot_range[2][1], (nsamples,))

    sx = np.random.uniform(sides_range[0][0], sides_range[0][1], (nsamples,))
    sy = np.random.uniform(sides_range[1][0], sides_range[1][1], (nsamples,))
    if on_ground:
        sz = cz * 2
    else:
        sz = np.random.uniform(sides_range[2][0], sides_range[2][1], (nsamples,))

    out = [Cuboid((cx[i], cy[i], cz[i]), (sx[i], sy[i], sz[i]), (rx[i], ry[i], rz[i])) for i in range(nsamples)]
    return out
    

def sample_sphere(N, center_range, rad_range, on_ground, nsamples):

    cx = np.random.uniform(center_range[0][0], center_range[0][1], (nsamples,))
    cy = np.random.uniform(center_range[1][0], center_range[1][1], (nsamples,))
    if on_ground:
        cz = np.random.uniform(center_range[2][0], min(center_range[2][1], N[2]/2), (nsamples,))
    else:
        cz = np.random.uniform(center_range[2][0], center_range[2][1], (nsamples,))

    if on_ground:
        rad = cz
    else:
        rad = np.random.uniform(rad_range[0], rad_range[1], (nsamples,))

    out = [Sphere((cx[i], cy[i], cz[i]), rad[i]) for i in range(nsamples)]
    return out

var_to_sampler = {
    'linscale': sample_linscale,
    'uniform': sample_uniform,
    'normal': sample_normal,
    'collection': sample_collection,
    'cuboid': sample_cuboid,
    'sphere': sample_sphere
}


def to_json_variable(var):
    if hasattr(var, 'to_dict'):
        return var.to_dict()
    else:
        return var

def json_insert(json, var, path):
    parts = path.split('.')
    root = json
    for i, part in enumerate(parts[:-1]):
        if isinstance(root, list):
            key = int(parts[i])
            try:
                _ = int(parts[i+1])
                root.insert(0, [])
            except:
                root.insert(0, dict())
        elif isinstance(root, dict):
            key = part
            if key not in root.keys():
                try:
                    _ = int(parts[i+1])
                    root[key] = []
                except:
                    root[key] = dict()
        root = root[key]
    try:
        key = int(parts[-1])
        root.insert(0, var)
    except:
        key = parts[-1]
        root[key] = var

        
def variable_sampling(var_block, var_path=""):
    if isinstance(var_block, list):
        var_block = {str(i):var_block[i] for i in range(len(var_block))}
    if "type" in var_block.keys():
        sampler = var_to_sampler[var_block["type"]]
        params = {k:var_block[k] for k in var_block.keys() if k != "type"}
        values = sampler(**params)
        return [values], [var_path]
    else:
        var_values, var_paths = [], []
        for k in var_block.keys():
            vpath = var_path + "." + k if var_path != "" else k
            vval, vpath = variable_sampling(var_block[k], vpath)
            var_values += vval
            var_paths += vpath

        return var_values, var_paths


def make_scenes(config, root):
    
    const_block = config["constants"]

    var_block = config["variables"]
    var_values, var_paths = variable_sampling(var_block)

    for combination in itertools.product(*var_values):
        out_json = copy.deepcopy(const_block)
        scene_folder = root / shake_256("".join([str(x) for x in combination]).encode('utf-8')).hexdigest(16)
        scene_folder.mkdir(exist_ok=True)
        for i, name in enumerate(var_paths):
            val = to_json_variable(combination[i])
            json_insert(out_json, val, name)
        out_json["export"]["out_dir"] = str(scene_folder.absolute())
        json.dump(out_json, open(scene_folder / 'config.json', 'w'), indent=4)


def generate(root, stdout):
    for sim_dir in root.iterdir():
        print("Simulating scene: {}".format(str(sim_dir)))
        command = "{} {}".format(SIMULATE_EXEC, str(sim_dir / "config.json"))
        subprocess.call(command, stdout=stdout)


if __name__ == "__main__":

    config = json.load(open(sys.argv[1], 'r'))

    print("Generating scene configuration files...")

    root_path = pathlib.Path(config["export_root"])
    root_path.mkdir(exist_ok=True, parents=True)

    if "seed" in config.keys():
        np.random.seed(config["seed"])
    make_scenes(config, root_path)

    log_path = root_path / "log.txt"
    log_path.touch(exist_ok=True)
    log = open(log_path, 'a')

    print("Simulating generated scenes...")

    generate(root_path, log)