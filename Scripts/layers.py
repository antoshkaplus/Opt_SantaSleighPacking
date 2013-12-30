
import pickle
from sys import argv

class Pr:
    def __init__(self, x, y, z, sz_x, sz_y, sz_z): 
        self.x = x
        self.y = y
        self.z = z
        self.sz_x = sz_x
        self.sz_y = sz_y
        self.sz_z = sz_z
    

if __name__ == "__main__":
    layers = []
    f = open(argv[1])
    f.readline()
    layer_z = 0
    for line in f:
        cs = map(int, line.split(","))
        id = cs[0]
        min_x = min(cs[1::3]); max_x = max(cs[1::3])
        min_y = min(cs[2::3]); max_y = max(cs[2::3])
        min_z = min(cs[3::3]); max_z = max(cs[3::3])
       
        if (max_z != layer_z):
            layer_z = max_z
            layers.append([])
        layers[-1].append(Pr(min_x, min_y, min_z, max_x-min_x+1, max_y-min_y+1, max_z-min_z+1))

    pickle.dump(layers, open("presents.pydb", "wb"))


    
    