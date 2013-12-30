
import numpy as np

class Pr:
    def __init__(self, r = 0, c = 0):
        self.r = r
        self.c = c
    def __eq__(self, other): 
        return self.r == other.r and self.c == other.c
    def set(self, r, c):
        self.r = r
        self.c = c
    def __str__(self):
        return "(" + str(self.r) + ", " + str(self.c) + ") "

def less(h_1, h_2):
    return h_1.r <= h_2.r and h_1.c <= h_2.c and (h_1.r != h_2.r or h_1.c != h_2.c)

HOR = 0
VER = 1


def print_coord_table(coord_table):
    n, n = coord_table.shape
    for r in range(n):
        for c in range(n):
            print str(coord_table[r,c][HOR]) + "|" + str(coord_table[r,c][VER]),
        print

def find_rects(bool_table, n):
    coord_table = np.zeros(dtype=list, shape = (n,n))
    for r in range(n):
        for c in range(n):
            # would be nice to place there object instead of tuples
            coord_table[r,c] = [Pr(), Pr()]
    
    for r in range(n):
        for c in range(n): 
            h = coord_table[r,c]
            if bool_table[r,c] == False:
                h[0], h[1] = Pr(), Pr()
                continue
        
            h_left = Pr() if c == 0 else coord_table[r,c-1][HOR]
            h_top = Pr() if r == 0 else coord_table[r-1,c][HOR]
            
            hh = h[HOR]
            
            if (h_left == Pr()): hh.set(1,1)
            else:
                hh.set(1, h_left.c+1)
                if (h_top.c == hh.c):
                    hh.r = min(h_left.r, h_top.r+1)
                        
            h_left = Pr() if r == 0 else coord_table[r,c-1][VER]
            h_top = Pr() if c == 0 else coord_table[r-1,c][VER]

            hh = h[VER]

            if (h_top == Pr()): hh.set(1,1)
            else:
                hh.set(h_top.r+1, 1)
                if (h_left.r == hh.r):
                    hh.c = min(h_left.c+1, h_top.c)
            
            if less(h[HOR], h[VER]): h[HOR] = h[VER]
            if less(h[VER], h[HOR]): h[VER] = h[HOR]
    return coord_table
    
n = 10
bool_table = np.random.random((n, n)) < 0.5 
print bool_table   
print_coord_table(find_rects(bool_table, 10))
    