import sys
import json
import pathlib
import msgpack
import msgpack_numpy
msgpack_numpy.patch()
import numpy as np
import zstandard as zstd
from load_phi import load_particles




if __name__ == "__main__":

    root = pathlib.Path(sys.argv[1])
    out_dir = root / 'msgpack'
    out_dir.mkdir(parents=True, exist_ok=True)
    fps = 50
    box_face_pts = 6500
    compressor = zstd.ZstdCompressor(level=22)

    for i, sim in enumerate(root.iterdir()):

        if sim.name == 'log.txt' or sim.name == 'msgpack':
            continue

        config = json.load(open(sim / 'config.json', 'r'))
        current_second = []
        frames = sim / 'frames'
        j = 0
        for frame in frames.iterdir():

            current_frame = {}
            pos, vel, nu, m = load_particles(frame)

            if j == 0:
                Nx, Ny, Nz = config['sim_params']['Nx'], config['sim_params']['Ny'], config['sim_params']['Nz']
                max_dim = float(max(max(Nx, Ny), Nz))
                box = [[-1.0, Nx / max_dim], [-1.0, Ny / max_dim], [-1.0, Nz / max_dim]]
                # fix one, sample uniformly samples_per_box_face points varying other 2 coordinates
                # normals are 1 for fixed coordinate min, -1 for fixed coordinate max, 0 for other 2 coords
                x_samples = np.random.uniform(box[0][0], box[0][1], (4, box_face_pts, 1))
                y_samples = np.random.uniform(box[1][0], box[1][1], (4, box_face_pts, 1))
                z_samples = np.random.uniform(box[2][0], box[2][1], (4, box_face_pts, 1))
                x_min, x_max = np.full((box_face_pts, 1), box[0][0]), np.full((box_face_pts, 1), box[0][1])
                y_min, y_max = np.full((box_face_pts, 1), box[1][0]), np.full((box_face_pts, 1), box[1][1])
                z_min, z_max = np.full((box_face_pts, 1), box[2][0]), np.full((box_face_pts, 1), box[2][1])
                xyz = np.concatenate([
                    np.concatenate([x_min, y_samples[0], z_samples[0]], axis=-1),
                    np.concatenate([x_max, y_samples[1], z_samples[1]], axis=-1),
                    np.concatenate([x_samples[0], y_min, z_samples[2]], axis=-1),
                    np.concatenate([x_samples[1], y_max, z_samples[3]], axis=-1),
                    np.concatenate([x_samples[2], y_samples[2], z_min], axis=-1),
                    np.concatenate([x_samples[3], y_samples[3], z_min], axis=-1)
                ], axis=0)
                normals = np.concatenate([
                    np.array([[1.0, 0.0, 0.0]]).repeat(box_face_pts, axis=0),
                    np.array([[-1.0, 0.0, 0.0]]).repeat(box_face_pts, axis=0),
                    np.array([[0.0, 1.0, 0.0]]).repeat(box_face_pts, axis=0),
                    np.array([[0.0, -1.0, 0.0]]).repeat(box_face_pts, axis=0),
                    np.array([[0.0, 0.0, 1.0]]).repeat(box_face_pts, axis=0),
                    np.array([[0.0, 0.0, -1.0]]).repeat(box_face_pts, axis=0),
                ], axis=0)

                current_frame['box'] = xyz.astype(np.float32)
                current_frame['box_normals'] = normals.astype(np.float32)

            current_frame['pos'] = pos.astype(np.float32)  ## TODO: ALL FLOAT32
            current_frame['vel'] = vel.astype(np.float32)
            current_frame['viscosity'] = nu.astype(np.float32)
            current_frame['m'] = m.astype(np.float32)
            current_frame['frame_id'] = j
            current_frame['scene_id'] = 'sim_{}'.format(str(i+1).zfill(4))
            s = j // fps

            current_second.append(current_frame)
            if j == fps or (j > fps and len(current_second) == fps):
                fname = out_dir / '{}_{}.msgpack.zst'.format(current_frame['scene_id'], str(s-1).zfill(2))
                with open(fname, 'wb') as f:
                    f.write(compressor.compress(msgpack.packb(current_second, use_bin_type=True)))
                current_second = []
            
            j += 1