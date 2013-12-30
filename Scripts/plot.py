
from layers import pickle, Pr
from sys import argv
layers = pickle.load(open(argv[1], "rb"))

from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
import numpy as np

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

lr = layers[int(argv[2])]

max_sz_z = max(map(lambda p: p.z+p.sz_z-1, lr))

base = filter(lambda p: p.z+p.sz_z-1==max_sz_z, lr)
tail = filter(lambda p: p.z+p.sz_z-1!=max_sz_z, lr)

xpos, ypos, zpos, dx, dy, dz = zip(*map(lambda p: (p.x, p.y, p.z, p.sz_x, p.sz_y, p.sz_z), base))
ax.bar3d(xpos, ypos, zpos, dx, dy, dz, color='b')

if tail != []:
    xpos, ypos, zpos, dx, dy, dz = zip(*map(lambda p: (p.x, p.y, p.z, p.sz_x, p.sz_y, p.sz_z), tail))
    ax.bar3d(xpos, ypos, zpos, dx, dy, dz, color='r')


plt.show()