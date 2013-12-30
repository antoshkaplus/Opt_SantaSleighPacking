

import matplotlib.pyplot as plt

f = open("stats.txt")
f.readline()

n_s = []
sz_z_s = []
for line in f: 
    n, sz_z = map(int, line.split(","))
    n_s.append(n)
    sz_z_s.append(sz_z)
    
plt.plot(n_s, sz_z_s)
plt.show()

