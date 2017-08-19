//
//  Layer.h
//  include
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
#include "MaxRectPacking.h"
#include "TabuSearchMaxRectPacking.h"
#include "TabuSearch.h"

const function<double(PR_C_IT, unsigned)> perf = performance;
const function<u_long(PR_C_IT, unsigned)> volm = volume; 

typedef vector<Pr>::iterator PR_IT;

struct BaseLayer : Layer {
public:
    
    // sz_z, n elements to pack
    typedef tuple<unsigned,unsigned> Trial;

    struct SolutionSearch {
        BaseLayer& master; 
        vector<Pr>& solution;
        SolutionSearch(BaseLayer& master) : master(master), solution(master.solution) {}
        virtual void search() = 0;
        static unsigned estimatePerfectHeight(PR_IT begin, unsigned n_max) {
            return ceil((double)volume(begin, n_max)/(SZ*SZ));
        }
    };
    
    struct PerfectSearch {
        
    };
    
    struct RecursiveSearch : SolutionSearch {
        RecursiveSearch(BaseLayer& master) : SolutionSearch(master) {}
        //static const unsigned sz = 10;
        void search() {}
        
        // make volume and performance as method
        struct Best {
            Best(PR_IT b, unsigned n, unsigned sz_z) : n(n), sz_z(sz_z) {
                volume = volm(b, n);
                _performance = performance();
            }
            void merge(Best& b) {
                n += b.n;
                sz_z += b.sz_z;
                volume += b.volume;
                _performance = performance();
            }
            
            unsigned n;
            unsigned sz_z;
            u_long volume;
            double _performance;
            
            double performance() const{
                return (double)volume/(double)(SZ*SZ*sz_z);
            }
            
            static bool less(const Best& b_1, const Best& b_2) {
                return b_1.performance() < b_2.performance();
            }
            static bool greater(const Best& b_1, const Best& b_2) {
                return b_1.performance() > b_2.performance() || (b_1.performance() == b_2.performance() && (b_1.sz_z < b_2.sz_z || (b_1.sz_z == b_2.sz_z && b_1.n > b_2.n)));
            }

        };
        static void estimate(vector<RecursiveSearch::Best>& best, unsigned n_best, PR_IT begin, unsigned n_max) {
            best.clear();
            unsigned n_cur;
            unsigned sz_z = maxSz(begin, boundSzZ(begin, n_max, 250));
            unsigned i = 0;
            for (; sz_z >= 1; sz_z--) {
                i++;
                n_cur = boundSzZ(begin, n_max, sz_z);
                // assert(n_cur != 0);
                // performance computes wrong way
                Best b(begin, n_cur, sz_z);
                if (n_cur > 0 && maxSzZ(begin, n_cur) == sz_z && 
                    (best.size() < n_best || b.performance() > best.back().performance())) {
                    if (best.size() == n_best) best.pop_back();
                    best.insert(lower_bound(best.begin(), best.end(), b, Best::greater), b);
                }
            }
        }
        static unsigned estimatePerfectHeight(PR_IT begin, unsigned n_max) {
            vector<Best> best;
            vector<RecursiveSearch::Best> buf; 
            vector<Best> manyBest;
            best.push_back(Best(begin, 0, 0));
            while (best.front().n != n_max) {
                manyBest.clear();
                for (Best& b : best) {
                    estimate(buf, 10, begin+b.n, n_max-b.n);
                    for (Best& lb : buf) {
                        lb.merge(b);
                        manyBest.push_back(lb);
                    }
                }
                sort(manyBest.begin(), manyBest.end(), Best::greater);
                best.assign(manyBest.begin(), manyBest.begin() + min<u_long>(manyBest.size(), 10));
            }
            return best.front().sz_z;
        }
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
                    //writePrs2D(begin, n);
                    if (result && (curPerf = perf(begin, n)) > bestPerf) {
                        bestPerf = curPerf;
                        solution.assign(begin, begin+n);
                    }
                    bs.update(result);
                }
            }
        }
        static unsigned estimatePerfectHeight(PR_IT begin, unsigned n_max) {
            unsigned n_cur, sz = 0;
            double best_perf, cur_perf;
            unsigned n_best, sz_best;
            auto b = begin;
            while (b != begin+n_max) {
                best_perf = 0;
                unsigned sz_z = maxSz(b, boundSzZ(b, n_max-(unsigned)(b-begin), 250));
                for (; sz_z >= 1; sz_z--) {
                    n_cur = boundSzZ(b, n_max-(unsigned)(b-begin), sz_z);
                    // performance computes wrong way
                    if ((cur_perf = perf(b, n_cur)) > best_perf) {
                        best_perf = cur_perf;
                        n_best = n_cur;
                        sz_best = sz_z;
                    }
                }
                sz += sz_best;
                b += n_best;
            }
            return sz;
        }
    };
    
    struct MaxCountSequentialSearch : SolutionSearch {
        double average_performance;
        MaxCountSequentialSearch(BaseLayer& master) : SolutionSearch(master) {
            average_performance = 0.99;
        }
        void search() {
            solution.clear();
            auto& packing = *master.packing;
            auto begin = master.begin;
            auto n_max = master.n_max;
            unsigned n, n_ave, n_cur;
            n = boundSzZ(begin, n_max, 250);
            n_ave = boundSzZ(begin, n_max, 250, average_performance);
            if (packing.fillFixed(begin, n) != 0) {
                n--;
                n_cur = n_ave;
                while (packing.fillFixed(begin, n_cur) != 0) n_cur--;
                if (n_cur == n_ave) { 
                    n_cur++;
                    while (n_cur <= n && packing.fillFixed(begin, n_cur) == 0) n_cur++;
                    n_cur--;
                }
            }
            else n_cur = n;
            cout << "needed: " << n << ", inside: " << n_cur << endl << endl;
            writePrs2D(begin, n);
            solution.assign(begin, begin+n_cur);
        }
        static unsigned estimatePerfectHeight(PR_IT begin, unsigned n_max) {
            unsigned n_cur, sz = 0;
            auto b = begin;
            while (b != begin+n_max) {
                n_cur = boundSzZ(b, n_max-(unsigned)(b-begin), 250);
                sz += maxSzZ(b, n_cur);
                b += n_cur;
            }
            return sz;
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

    BaseLayer() : bruteSearch(*this), maxCountSequentialSearch(*this) {
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
    vector<Pr> solution;
    BruteSearch bruteSearch;
    MaxCountSequentialSearch maxCountSequentialSearch;
    
    LinePacking linePacking;
    CornerPacking cornerPacking;
    MaxRectPacking maxRectPacking;
    TabuSearchMaxRectPacking tabuSearchMaxRectPacking;
};


#endif /* defined(__SantaSleighPacking__Layer__) */
