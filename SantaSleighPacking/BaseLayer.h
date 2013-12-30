//
//  Layer.h
//  SantaSleighPacking
//
//  Created by Anton Logunov on 12/11/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#ifndef __SantaSleighPacking__Layer__
#define __SantaSleighPacking__Layer__

#include <map>
#include <algorithm>
#include <queue>
#include <cmath>
#include <functional>

#include "Help.h"
#include "Shelf.h"
#include "Packing.h"

const function<double(PR_C_IT, unsigned)> perf = performance;

typedef vector<Pr>::iterator PR_IT;

struct BaseLayer : Layer {
public:
    
    // sz_z, n elements to pack
    typedef tuple<unsigned,unsigned> Trial;

    struct SolutionSearch {
        BaseLayer& master; 
        vector<Pr> solution;
        SolutionSearch(BaseLayer& master) : master(master) {}
        virtual void search() = 0;
    };
    
    struct PerfectSearch {
        
    };
    
    struct BruteSearch : SolutionSearch {
        BruteSearch(BaseLayer& master) : SolutionSearch(master) {}
        void search() {
            solution.clear();
            auto& packing = *master.packing;
            auto begin = master.begin;
            auto n_max = master.n_max;
            unsigned n, n_bound;
            bool result;
            double curPerf, bestPerf = 0;
            BinarySearch bs;
            n = boundSzZ(begin, n_max, 250);
            for (unsigned sz_z = maxSz(begin, n); sz_z >= 1; sz_z--) {
                n_bound = boundSzZ(begin, n_max, sz_z);
                // should use continue instead of break...
                if (perf(begin, n_bound) <= bestPerf) continue; 
                bs.init(0, n_bound);
                while (!bs.isFinished()) {
                    n = bs.value();
                    result = packing.fillFixed(begin, n) == 0;
                    if (result && (curPerf = perf(begin, n)) > bestPerf) {
                        bestPerf = curPerf;
                        solution.assign(begin, begin+n);
                    }
                    bs.update(result);
                }
            }
            
        }
        
    };
    
    
    unsigned fill(PR_IT begin, unsigned n_max) {
        this->begin = begin;
        this->n_max = n_max;
        solutionSearch->search();
        n = (unsigned)solutionSearch->solution.size();
        copy_n(solutionSearch->solution.begin(), n, begin);
        sz_z_max = maxSzZ(begin, n);
        for (PR_IT b_it = begin; b_it != begin+n; b_it++) {
            Pr &pr = *b_it;
            pr.z = 1 + sz_z_max - pr.sz_z;
        }
        return n;
    }

    BaseLayer() : bruteSearch(*this) {
        packing = &maxRectPacking;
        solutionSearch = &bruteSearch;
    }
    
    void setPacking(Packing& p) {
        packing = &p;
    }
    
    void setSearch(SolutionSearch& s) {
        solutionSearch = &s;
    }

    unsigned n_max;
    Packing *packing;
    SolutionSearch *solutionSearch;
    
    BruteSearch bruteSearch;
    
    LinePacking linePacking;
    CornerPacking cornerPacking;
    MaxRectPacking maxRectPacking;
    TabuSearchMaxRectPacking tabuSearchMaxRectPacking;
};


#endif /* defined(__SantaSleighPacking__Layer__) */