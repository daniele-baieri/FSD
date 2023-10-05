import sys
import vedo
import pathlib
import struct
import time
import numpy as np
from tqdm import tqdm



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

    path = pathlib.Path(sys.argv[2])

    fluid_type = sys.argv[1]
    sim = pathlib.Path(sys.argv[2])
    fps = 50
    plt = vedo.Plotter(bg2='lb', interactive=False, size=(1280, 768), 
                        axes=dict(
                            xrange=(-1, 1), yrange=(-1, 1), zrange=(-1, 1),
                            xygrid=True, yzgrid=True, zxgrid=True
                        )
                       )

    first = True
    for frame in tqdm((sim / 'frames').iterdir()):
    
        if fluid_type == 'particles':
            pos, vel, nu, m = load_particles(frame)
            v_view = vedo.Arrows(pos, pos + vel)
            if first:
                first = False
                p_view = vedo.Points(pos)
                plt.show(p_view, v_view, viewup='z')
            else:
                plt.show(v_view, resetcam=False)
                p_view.points(pos)
            plt.render(resetcam=False)
            plt.remove(v_view)

        time.sleep(1/fps)