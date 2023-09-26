import sys
import vedo
import pathlib
import struct
import numpy as np



def load_phi(path):

    f = open(path, 'rb')
    xyz = struct.unpack("iii", f.read(4 * 3))
    nnz = struct.unpack("i", f.read(4))[0]
    idxs = np.array(struct.unpack("i" * nnz, f.read(4 * nnz)))
    vals = np.array(struct.unpack("f" * nnz, f.read(4 * nnz)), dtype=np.float32)

    out = np.zeros(xyz, dtype=np.float32)

    t = np.mod(idxs, np.array([xyz[0] * xyz[1]]))

    x = t % xyz[0]
    y = t // xyz[0]
    z = idxs // (xyz[0] * xyz[1])

    out[x, y, z] = vals
    return out

def load_particles(path):

    f = open(path, 'rb')
    xyz = struct.unpack("iii", f.read(4 * 3))
    num_part = struct.unpack("i", f.read(4))[0]
    vals = np.array(struct.unpack("f" * 8 * num_part, f.read(8 * 4 * num_part))).reshape(num_part, 8)
    pos, vel, nu, m = vals[:, :3], vals[:, 3:6], vals[:, 6:7], vals[:, 7:]

    max_dim = max(xyz)
    pos /= (max_dim / 2)

    return pos, vel, nu, m




if __name__ == "__main__":

    fluid_type = sys.argv[1]
    path = pathlib.Path(sys.argv[2])

    if fluid_type == 'density':
        vol = load_phi(path)
        vol = vedo.Volume(vol, c=['white','b','g','r'], mode=1)
        plt = vedo.applications.IsosurfaceBrowser(vol, use_gpu=True, c='gold')
        plt.show(axes=7, bg2='lb').close()
    elif fluid_type == 'particles':
        pos, vel, nu, m = load_particles(path)
        p_view = vedo.Points(pos)
        vel = vedo.Arrows(pos, pos + vel)
        vedo.show(p_view, vel, bg2='lb')
    else:
        print("Unknown fluid type: {}".format(fluid_type))
        sys.exit(1)