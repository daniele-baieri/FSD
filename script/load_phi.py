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






if __name__ == "__main__":

    path = pathlib.Path(sys.argv[1])

    vol = load_phi(path)

    vol = vedo.Volume(vol, c=['white','b','g','r'], mode=1)
    plt = vedo.applications.IsosurfaceBrowser(vol, use_gpu=True, c='gold')
    plt.show(axes=7, bg2='lb').close()