
from layers import Pr
from glob import glob
import pickle 

layers = []
for file_name in glob("layer-*.txt"): 
    layers.append([])
    for line in open(file_name):
        layers[-1].append(Pr(*map(int, line.split(","))[1:]))
pickle.dump(layers, open("presents.pydb", "wb"))
