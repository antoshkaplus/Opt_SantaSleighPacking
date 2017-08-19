//
//  main.cpp
//  include
//
//  Created by Anton Logunov on 12/10/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#include <numeric>

#include "../SantaSleighPacking/BaseLayer.h"
#include "../SantaSleighPacking/TailLayer.h"

struct Best {
    unsigned sz_z_max;
    double performance;
    vector<Pr> prs;
    void set(double performance, unsigned sz_z_max) { //, PR_IT b, unsigned n) {
        this->performance = performance;
        this->sz_z_max = sz_z_max; 
        //prs.assign(b, b+n);
    }
};

const bool COMPUTE_TAIL = false;
const bool WRITE_LOG = true;
const bool WRITE_STATS = true; 

struct SantaSleigh {
    vector<Pr>::iterator b_it; // WORKING WITH SECOND LAYER
    vector<unsigned> szs;
    Layer layer; // don't forget to initialize me with begin
    BaseLayer baseL;
    
    void willPackBig() {
        baseL.setSearch(baseL.maxCountSequentialSearch);
        baseL.setPacking(baseL.tabuSearchMaxRectPacking);
        //baseL.tabuSearchMaxRectPacking.setNext(baseL.tabuSearchMaxRectPacking.nextMaxHuge);
   }
    
    void willPackMedium() {
        willPackBig();
    }
    
    void willPackSmall() {
        baseL.setSearch(baseL.bruteSearch);
        baseL.maxRectPacking.setNext(baseL.maxRectPacking.nextMaxHuge);
    }

    void pack(vector<Pr>::iterator begin, unsigned n) {
        // begin 1, 2, 3, 4....
        //TailLayer tailL;
      
        bool startedPackBig = false;
        bool startedPackMedium = false;
        bool startedPackSmall = false;
        
        ofstream stats;
        if (WRITE_STATS) {
            stats.open("stats.txt");
            stats << "elelements_inside,size_z" << endl;
        }
        b_it = begin;
        layer.begin = b_it;
        while (b_it-begin != n) {
            if (b_it->id >= 1 && !startedPackMedium) {
                willPackMedium();
                startedPackMedium = true;
            } 
            if (b_it->id >= 400000 && !startedPackBig) {
                willPackBig();
                startedPackBig = true;
            } 
            if (b_it->id >= 700000-200 && !startedPackSmall) {
                willPackSmall();
                startedPackSmall = true;
            }
            baseL.fill(b_it, n-(unsigned)(b_it-begin));
            checkLayer(b_it, baseL.n, 1, baseL.sz_z_max);
            if (COMPUTE_TAIL) {
                /*
                tailL.fill(baseL, b_it+baseL.n, n-(unsigned)(b_it-begin)-baseL.n, tailL.rectSetStrategy);
                layer.putBelow(tailL);    
                b_it += tailL.n;
                */
            }
            else {
                layer.putBelow(baseL);
                b_it += baseL.n;
            }    
            if (WRITE_LOG) {
                //writePrs2D(baseL.begin, baseL.n);
                printf("elements inside: %u sz_z: %u\n", (unsigned)(b_it-begin), layer.sz_z_max);
            }
            if (WRITE_STATS) {
                stats << b_it-begin << "," << layer.sz_z_max << endl;
            }
        }
        if (WRITE_STATS) stats.close();
     }
     
     void test(vector<Pr>::iterator begin, unsigned n) {
         baseL.setSearch(baseL.maxCountSequentialSearch);
         baseL.setPacking(baseL.tabuSearchMaxRectPacking);
         
         b_it = begin + 438; // need 202
         unsigned nn = baseL.fill(b_it, n-(unsigned)(b_it-begin));
         writePrs2D(b_it, nn);
     }

};

int main(int argc, const char * argv[])
{
    SantaSleigh santaSleigh;
    vector<Pr> prs(N);
    readPrsSzs(prs.begin(), N);
    santaSleigh.pack(prs.begin(), N);
    //santaSleigh.test(prs.begin(), N);
    writePrsLocs(prs.begin(), N);
    // insert code here...
    std::cout << "Hello, World!\n";
    return 0;
}

