"""
Sample N camera positions and angles so that
1. They are equidistant from a given center
2. Their lookat point is the given center
3. They have positive elevation
"""
import numpy as np


if __name__ == "__main__":

    N = 10
    cam_dist = 384.0  # 70.0
    center = [0.0, 0.0, 0.0]
    bb = [-128.0, 128.0]
    min_z_offset = 25.0
    np.random.seed(107001)


    c = np.array(center)
    r = np.zeros((N,)) + cam_dist  # r spherical coord
    phi = np.random.uniform(0.0, 2*np.pi, (N,))  # sample any azimuth phi

    # to sample inclination, we first sample z coordinates
    z = np.random.uniform(center[2] + min_z_offset, center[2] + cam_dist, (N,))
    # then we turn them into inclinations on the canonical zero-centered sphere
    theta = np.arccos((z - c[2]) / r)  # this way we are sure to only have positive inclinations

    # now we compute x and y coordinates
    x = r * np.sin(theta) * np.cos(phi)
    y = r * np.sin(theta) * np.sin(phi)

    print("Camera positions:")
    print(np.array2string(np.stack([x, y, z], axis=-1), separator=","))

    print("Camera angles:")
    print(np.array2string(np.stack([90.0 + np.rad2deg(phi), 90.0 + np.rad2deg(theta)], axis=-1), separator=","))

    

