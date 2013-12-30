//
//  main.cpp
//  SantaSleighPacking
//
//  Created by Anton Logunov on 12/10/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#include <numeric>

#include "BaseLayer.h"
#include "TailLayer.h"

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

void fill(vector<Pr>::iterator begin, unsigned n) {
    // begin 1, 2, 3, 4....
    vector<Pr>::iterator b_it = begin; // WORKING WITH SECOND LAYER
    vector<unsigned> szs;
    Layer layer(begin);
    BaseLayer baseL;
    //TailLayer tailL;
    //baseL.setPacking(baseL.linePacking);
    //baseL.setPacking(baseL.maxRectPacking);
    
    /*
    baseL.tabuSearchMaxRectPacking.logEnabled = WRITE_LOG;
    baseL.tabuSearchMaxRectPacking.setChoosePair(baseL.tabuSearchMaxRectPacking.bestShortSideFit);
    baseL.tabuSearchMaxRectPacking.setInsert(baseL.tabuSearchMaxRectPacking.topLeft);
    baseL.setPacking(baseL.tabuSearchMaxRectPacking);
    */
    
    baseL.maxRectPacking.setNext(baseL.maxRectPacking.nextMaxHugeInSmalRect);
    //baseL.maxRectPacking.setNext(baseL.maxRectPacking.nextMaxHuge);
    //baseL.maxRectPacking.setCandidate(baseL.maxRectPacking.doubleOrigin);
    ofstream stats;
    if (WRITE_STATS) {
        stats.open("stats.txt");
        stats << "elelements_inside,size_z" << endl;
    }
    while (b_it-begin != n) {
        //if (b_it-begin > 700000) baseL.setPacking(baseL.linePacking);
        // all take what they need
        //baseL.setAdjustment(baseL.optimalAdjustment);
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
            printf("elements inside: %u sz_z_max: %u\n", (unsigned)(b_it-begin), baseL.sz_z_max);
        }
        if (WRITE_STATS) {
            stats << b_it-begin << "," << layer.sz_z_max << endl;
        }
    }
    if (WRITE_STATS) stats.close();
 }


int main(int argc, const char * argv[])
{
    vector<Pr> prs(N);
    readPrsSzs(prs.begin(), N);
    fill(prs.begin(), N);
    writePrsLocs(prs.begin(), N);
    // insert code here...
    std::cout << "Hello, World!\n";
    return 0;
}

