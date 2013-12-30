//
//  Help.h
//  SantaSleighPacking
//
//  Created by Anton Logunov on 12/11/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#ifndef SantaSleighPacking_Help_h
#define SantaSleighPacking_Help_h


#include <algorithm>
#include <iostream>
#include <vector>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <tuple>
#include <list>
#include <cassert>
#include <random>

using namespace std;

const unsigned int N = 1000000;
const unsigned int SZ = 1000; 

struct PlaneLoc {
    PlaneLoc() {}
    PlaneLoc(unsigned x, unsigned y) : x(x), y(y) {}
    unsigned x, y;
};

struct SpaceLoc : PlaneLoc {
    SpaceLoc() {}
    SpaceLoc(unsigned x, unsigned y, unsigned z) : PlaneLoc(x, y), z(z){}
    unsigned z;
};

struct Rect {
    Rect() {}
    Rect(unsigned sz_x, unsigned sz_y) : sz_x(sz_x), sz_y(sz_y) {}
    unsigned sz_x, sz_y;
    
    unsigned area() const {
        return sz_x*sz_y;
    }
    bool canInsert(const Rect& r) const {
        return sz_x >= r.sz_x && sz_y >= r.sz_y; 
    }
    bool canInsertRotated(const Rect& r) const {
        return sz_x >= r.sz_y && sz_y >= r.sz_x;
    }
    
    static bool areaLess(const Rect& r_1, const Rect& r_2) {
        return r_1.area() < r_2.area();
    }
};

struct LocRectInterface {};

struct LocRect : PlaneLoc, Rect {
    LocRect() {}
    LocRect(unsigned x, unsigned y, unsigned sz_x, unsigned sz_y) 
        : PlaneLoc(x, y), Rect(sz_x, sz_y) {}
        
    bool isIntersection(const LocRect& r) const {
        bool b = (x <= r.x+r.sz_x-1 && x+sz_x-1 >= r.x && 
                  y <= r.y+r.sz_y-1 && y+sz_y-1 >= r.y);
        return b;
    }
    
    bool hasInside(const LocRect& r) const {
        return x <= r.x && x+sz_x >= r.x+r.sz_x && 
               y <= r.y && y+sz_y >= r.y+r.sz_y; 
    }
    
    static bool areAdjucent(const LocRect& r_1, const LocRect& r_2) {
        return ((r_1.x+r_1.sz_x==r_2.x || r_2.x+r_2.sz_x==r_1.x) &&
                    ((r_1.y <= r_2.y && r_2.y <= r_1.y+r_1.sz_y-1) || 
                     (r_2.y <= r_1.y && r_1.y <= r_2.y+r_2.sz_y-1))) 
                ||
               ((r_1.y+r_1.sz_y==r_1.y || r_2.y+r_2.sz_y==r_1.y) &&
                    ((r_1.x <= r_2.x && r_2.x <= r_1.x+r_1.sz_x-1) ||
                     (r_2.x <= r_1.x && r_1.x <= r_2.x+r_2.sz_x-1)));
    }
};

struct Cuboid : Rect {
    Cuboid() {}
    Cuboid(unsigned sz_x, unsigned sz_y, unsigned sz_z) : Rect(sz_x, sz_y), sz_z(sz_z){}
    unsigned sz_z;
    
    unsigned volume() const {
        return area()*sz_z;
    }
};

struct LocCuboid : SpaceLoc, Cuboid {
    bool isIntersection(const LocCuboid& p) const {
        bool r = (x <= p.x+p.sz_x-1 && x+sz_x-1 >= p.x && 
                  y <= p.y+p.sz_y-1 && y+sz_y-1 >= p.y &&
                  z <= p.z+p.sz_z-1 && z+sz_z-1 >= p.z);
        return r;
    }
    
    LocRect toLocRect() {
        return LocRect(x, y, sz_x, sz_y);
    }
};

struct Pr : LocCuboid {
    unsigned id;
    bool isIntersection(const Pr& p) const {
        bool r = LocCuboid::isIntersection(p);
        if (r) {
            printf("%u intersects %u\n", id, p.id);
        }
        return r;
    }
    void fromLocRect(const LocRect& r) {
        x = r.x;
        y = r.y;
        sz_x = r.sz_x;
        sz_y = r.sz_y;
    }
};

typedef vector<Pr>::iterator PR_IT;
typedef vector<Pr>::const_iterator PR_C_IT;

void readPrsSzs(PR_IT begin, unsigned n) {
    FILE *file = fopen("presents.csv", "r");
    assert(file!=nullptr);
    char str[100];
    fscanf(file, "%s", str);
    for (unsigned i = 0; i < n; i++) {
        Pr &pr = *(begin++);
        fscanf(file, "%u,%u,%u,%u", &pr.id, &pr.sz_x, &pr.sz_y, &pr.sz_z);
    };
    fclose(file);
}
void writePrsLocs(PR_C_IT begin, unsigned n) {
    FILE *file = fopen("presents.txt", "w");
    assert(file!=nullptr);
    fprintf(file, "PresentId,x1,y1,z1,x2,y2,z2,x3,y3,z3,x4,y4,z4,x5,y5,z5,x6,y6,z6,x7,y7,z7,x8,y8,z8\n");
    for (unsigned i = 0; i < n; i++) {
        const Pr &pr = *(begin++);
        unsigned x_1 = pr.x, x_2 = x_1+pr.sz_x-1, 
                 y_1 = pr.y, y_2 = y_1+pr.sz_y-1, 
                 z_1 = pr.z, z_2 = z_1+pr.sz_z-1;
        array<SpaceLoc, 8> ps = {SpaceLoc(x_1, y_1, z_1), SpaceLoc(x_2, y_2, z_2), SpaceLoc(x_1, y_2, z_1), 
                                 SpaceLoc(x_1, y_2, z_2), SpaceLoc(x_2, y_1, z_1), SpaceLoc(x_2, y_2, z_1),
                                 SpaceLoc(x_1, y_1, z_2), SpaceLoc(x_2, y_1, z_2)}; 
        
        fprintf(file, "%u", pr.id);
        for_each(ps.begin(), ps.end(), [file](const SpaceLoc& p) {
            fprintf(file, ",%u,%u,%u", p.x, p.y, p.z);
        });
        fprintf(file, "\n");
    }
    fclose(file);
}
void writePrsLayer(PR_C_IT begin, unsigned n) {
    string file_name = "layer-" + to_string(begin->id) + ".txt";
    FILE *file = fopen(file_name.c_str(), "w");
    assert(file!=nullptr);
    for (auto b = begin; b != begin+n; b++) {
        const Pr &p = *b;
        fprintf(file, "%u, %u, %u, %u, %u, %u, %u\n", p.id, p.x, p.y, p.z, p.sz_x, p.sz_y, p.sz_z);
    }
    fclose(file);
}
void writePrs2D(PR_C_IT begin, unsigned n) {
    FILE *file = fopen("2d.txt", "w");
    assert(file!=nullptr);
    fprintf(file, "%u,%u\n", SZ, SZ);
    for (auto p_it = begin; p_it != begin+n; p_it++) {
        fprintf(file, "%u,%u,%u,%u\n", p_it->x, p_it->y, p_it->x+p_it->sz_x-1, p_it->y+p_it->sz_y-1);
    }
    fclose(file);
}

void checkLayer(PR_C_IT begin, unsigned n, unsigned z_min, unsigned z_max) {
    for (PR_C_IT b = begin; b != begin+n; b++) {
        const Pr &p = *b;
        if (!(p.x >= 1 && p.x+p.sz_x-1 <= SZ && 
              p.y >= 1 && p.y+p.sz_y-1 <= SZ && 
              p.z >= z_min && p.z+p.sz_z-1 <= z_max)) {
            printf("out of bounds %u\n", begin->id);
            assert(0);
        }
        for (PR_C_IT e = b+1; e != begin+n; e++) {
            assert(!p.isIntersection(*e));
        }
    }
}

void collectSzs(vector<unsigned>& r, PR_C_IT b, unsigned n, unsigned min_sz) { 
    r.clear();
    auto e = b+n;
    while (b != e) {
        if (b->sz_x >= min_sz) r.push_back(b->sz_x);
        if (b->sz_y >= min_sz) r.push_back(b->sz_y);
        if (b->sz_z >= min_sz) r.push_back(b->sz_z);
        b++;
    }
    sort(r.begin(), r.end());
    r.erase(unique(r.begin(), r.end()), r.end());
}


struct BinarySearch {
    bool t;
    // need something to do with it
    int a, b, c;
    BinarySearch(unsigned a, unsigned b) : a(a), b(b) {
        updateValue();
    }
    BinarySearch() {}
    void init(unsigned a, unsigned b) {
        this->a = a;
        this->b = b;
        updateValue();
    }
    
    unsigned value() {
        assert(c >= 0);
        return c;
    }
    void update(bool t) {
        if (t) a = c+1;
        else b = c-1;
        updateValue();
    }
    bool isFinished() {
        return a > b;
    }
private:
    void updateValue() {
        c = (a+b)/2;
    }
};


unsigned maxSzZ(PR_C_IT b, unsigned n) {
    unsigned max_sz = 0;
    PR_C_IT e = b+n;
    while (b != e) {
        max_sz = max(max_sz, (b++)->sz_z);
    }
    return max_sz;
}

unsigned maxSz(Pr& p) {
    return max({p.sz_x, p.sz_y, p.sz_z});
}
unsigned minSz(Pr& p) {
    return min({p.sz_x, p.sz_y, p.sz_z});
}
unsigned minSz(Pr* p) {
    return min({p->sz_x, p->sz_y, p->sz_z});
}

unsigned maxSz(PR_IT b, unsigned n) {
    unsigned m = 0;
    auto e = b + n;
    while (b != e) {
        m = max(m, maxSz(*(b++)));
    }
    return m; 
}


unsigned area(PR_C_IT b, unsigned n) {
    unsigned a = 0;
    PR_C_IT e = b+n;
    while (b != e) {
        a += (b++)->area();
    }
    return a;
}


unsigned volume(PR_C_IT b, unsigned n) {
    unsigned v = 0;
    PR_C_IT e = b+n;
    while (b != e) {
        v += (b++)->volume();
    }
    return v;
}

double performance(PR_C_IT b, unsigned n) {
    return (double)volume(b, n)/(maxSzZ(b, n)*SZ*SZ);
}

// optimal element size
// tuple<unsigned, unsigned>


// returns number of elements bounded (prepare for plane placing)
unsigned boundSzZ(PR_IT begin, unsigned n_max, unsigned sz_z_max) {
    array<unsigned, 3> arr, arrI = {0, 1, 2};
    function<bool(unsigned)> notFit = [&arr, &sz_z_max](unsigned i){ 
        return arr[i] > sz_z_max; 
    };
    function<bool(unsigned,unsigned)> comp = [&arr](unsigned i_1, unsigned i_2) {
        return arr[i_1] < arr[i_2];
    };
    unsigned area = 0, i;
    for (i = 0; i != n_max; i++) {
        Pr &p = *(begin+i);
        arr = {p.sz_x, p.sz_y, p.sz_z};
        // should work on that
        auto a_it = partition(arrI.begin(), arrI.end(), notFit);
        if (a_it == arrI.end()) break;
        unsigned i_a = *max_element(a_it, arrI.end(), comp);
        swap(arr[2], arr[i_a]);
        p.sz_x = arr[0];
        p.sz_y = arr[1];
        p.sz_z = arr[2];
        if ((area += p.area()) > SZ*SZ) break;
    }
    return i;
}


struct Layer {
    PR_IT begin;
    unsigned n;
    unsigned sz_z_max;
    Layer(PR_IT begin) : Layer() { this->begin = begin; }
    Layer() : n(0), sz_z_max(0) {} 
    
    void putBelow(Layer& lr) {
        // lift current layer
        for (PR_IT b = begin; b != begin+n; b++) {
            b->z += lr.sz_z_max;
        }
        sz_z_max += lr.sz_z_max;
        n += lr.n;
    }
    void putAbove(Layer& lr) {
        for (PR_IT b = lr.begin; b != lr.begin+lr.n; b++) {
            b->z += sz_z_max;
        }
        sz_z_max += lr.sz_z_max;
        n += lr.n;
        begin = lr.begin;
    }
    void lower(unsigned sz_z) {
        for (PR_IT b = begin; b != begin+n; b++) {
            b->z -= sz_z;
        }
        sz_z_max -= sz_z;
    }
    double performance() {
        return (double)volume(begin, n)/(SZ*SZ*sz_z_max);
    }
};


template<class T>
struct Grid { 
    T* grid; 
    Grid() : grid(new T[SZ*SZ]) {}
    const T& operator()(unsigned x, unsigned y) const {
        return grid[(y-1)*SZ + x-1];
    }
    T& operator()(unsigned x, unsigned y) {
        return grid[(y-1)*SZ + x-1];
    }
    T& at(unsigned x, unsigned y) {
        return grid[(y-1)*SZ + x-1];
    }
    void fill(T val) {
        std::fill(grid, grid+SZ*SZ, val);
    }
    ~Grid() {
        delete [] grid;
    }
};      


struct MaxRectFinder {
    Grid<unsigned> dN, dS;
    // grid values : is FILLED ? 
    void find(vector<LocRect>& rects, Grid<bool>& grid) {
        rects.clear();
        for (int y = 1; y <= SZ; y++) {
            for (int x = 1; x <= SZ; x++) {
                dN(x, y) = (grid(x, y)) ? 0 : (1 + ((y == 1) ? 0 : dN(x, y-1)));
            }
        }
        for (int y = SZ; y >= 1; y--) {
            for (int x = 1; x <= SZ ; x++) {
                dS(x, y) = (grid(x, y)) ? 0 : (1 + ((y == SZ) ? 0 : dS(x, y+1)));
            }
        }
        for (int x = SZ; x >= 1; x--) {
            int maxS = SZ+1;
            for (int y = SZ; y >= 1; y--) {
                maxS++;
                if (!grid(x, y) && (x == 1 || grid(x-1, y))) {
                    int N = dN(x, y);
                    int S = dS(x, y);
                    int width = 1;
                    while (x + width <= SZ && !grid(x + width, y)) {
                        int nextN = dN(x+width, y);
                        int nextS = dS(x+width, y);
                        if ((nextN < N) || (nextS < S)) {
                            if (S < maxS) {
                                rects.push_back(LocRect(x, y-N+1, width, N+S-1));
                            }
                            if (nextN < N) N = nextN;
                            if (nextS < S) S = nextS;
                        }
                        width++;
                    }
                    if (S < maxS) {
                        rects.push_back(LocRect(x, y-N+1, width, N+S-1));
                    }
                    maxS = 1; // maybe 1?
                }
            }
        }
    }

};


#endif
