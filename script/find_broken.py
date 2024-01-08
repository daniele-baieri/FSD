import sys
import pathlib
import argparse
import numpy as np
from tqdm import tqdm
from load_phi import load_particles



def rmtree(root):

    for p in root.iterdir():
        if p.is_dir():
            rmtree(p)
        else:
            p.unlink()

    root.rmdir()


if __name__ == "__main__":

    parser = argparse.ArgumentParser(
        prog='Find Broken Simulations',
        description='Finds simulations in a generated dataset which \
            count too many particles in a single grid cell (the error \
            state of the LBM simulator places all the particles at a \
            corner of the simulation domain).'
    )
    parser.add_argument('root')
    parser.add_argument('-d', '--delete', action='store_true')
    parser.add_argument('-g', '--grid_resolution', default=64)
    parser.add_argument('-t', '--threshold', default=200)
    parser.add_argument('-s', '--domain_world_size', default=2.0)
    args = parser.parse_args()

    root = pathlib.Path(args.root)
    delete = args.delete
    N = args.grid_resolution
    thresh = args.threshold
    dx = args.domain_world_size / N

    i = 0
    out = []
    for sim in tqdm(root.iterdir()):

        if sim.name in {'msgpack', 'find_broken.py', 'broken.txt', 'dam-break.zip'}:
            continue

        if not (sim / 'frames').exists():
            continue

        for particles in tqdm((sim / 'frames').iterdir(), leave=False):
            pos, _, _, _ = load_particles(particles)
            pos += 1.0  # shift to positive values
            idxs = np.floor(pos / dx)  # count how many dxs fit in each pos coordinate (= grid index for coordinate)
            idxs = (idxs[:, 0] * (N ** 2)) + (idxs[:, 1] * N) + idxs[:, 2]  # to linear indices
            _, counts = np.unique(idxs, return_index=False, return_inverse=False, return_counts=True)
            if np.any(counts > thresh):
                out.append("{}/{}".format(i+1, sim.name))
                break

        i += 1

    print("{} broken simulations:".format(len(out)))
    if delete:
        print("=== WARNING: ALL broken simulations will be removed. ===")
        ans = input("Continue? yes/no[yes] ===\n")
        if ans.lower() in ['yes', '']:
            pass
        else:
            delete = False
            if ans.lower() != 'no':
                print("Unknown answer \'{}\', not deleting".format(ans))
    for idxsim in out:
        print(idxsim)
        if delete:
            rmtree(root / idxsim.split('/')[1])