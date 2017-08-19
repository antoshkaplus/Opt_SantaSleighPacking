//
//  Help.h
//  include
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
    int x, y;
    static double distance(const PlaneLoc& p_1, const PlaneLoc& p_2) {
        return sqrt((p_1.x-p_2.x)*(p_1.x-p_2.x) + (p_1.y-p_2.y)*(p_1.y-p_2.y));
    }
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
    
    unsigned minSz() const {
        return min(sz_x, sz_y);
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
    
    static bool areAdjacent(const LocRect& r_1, const LocRect& r_2) {
        return ((r_1.x+r_1.sz_x==r_2.x || r_2.x+r_2.sz_x==r_1.x) &&
                    ((r_1.y <= r_2.y && r_2.y <= r_1.y+r_1.sz_y-1) || 
                     (r_2.y <= r_1.y && r_1.y <= r_2.y+r_2.sz_y-1))) 
                ||
               ((r_1.y+r_1.sz_y==r_1.y || r_2.y+r_2.sz_y==r_1.y) &&
                    ((r_1.x <= r_2.x && r_2.x <= r_1.x+r_1.sz_x-1) ||
                     (r_2.x <= r_1.x && r_1.x <= r_2.x+r_2.sz_x-1)));
    }
    
    PlaneLoc endLoc() const {
        return PlaneLoc(x+sz_x-1, y+sz_y-1);
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

void readPrsSzs(PR_IT begin, unsigned n);
void writePrsLocs(PR_C_IT begin, unsigned n);
void writePrsLayer(PR_C_IT begin, unsigned n);
void writePrsLayer(PR_C_IT begin, unsigned n);
void writePrs2D(PR_C_IT begin, unsigned n);
void checkLayer(PR_C_IT begin, unsigned n, unsigned z_min, unsigned z_max);
void collectSzs(vector<unsigned>& r, PR_C_IT b, unsigned n, unsigned min_sz);

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

unsigned maxSzZ(PR_C_IT b, unsigned n);
unsigned maxSz(Pr& p);
unsigned minSz(Pr& p);
unsigned minSz(Pr* p);
unsigned maxSz(PR_IT b, unsigned n);


unsigned area(PR_C_IT b, unsigned n);
u_long volume(PR_C_IT b, unsigned n);
double performance(PR_C_IT b, unsigned n);

unsigned boundSzZ(PR_IT begin, unsigned n_max, unsigned sz_z_max);
unsigned boundSzZ(PR_IT begin, unsigned n_max, unsigned sz_z_max, double performance);

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
