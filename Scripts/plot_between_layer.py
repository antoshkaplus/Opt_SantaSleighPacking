

from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
import numpy as np

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

lr = layers[-1]

xpos, ypos, zpos, dx, dy, dz = zip(*map(lambda p: (p.x, p.y, p.z, p.sz_x, p.sz_y, p.sz_z), lr))
ax.bar3d(xpos, ypos, zpos, dx, dy, dz, color='b')

plt.show()