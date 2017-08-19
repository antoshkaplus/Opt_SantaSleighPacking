//
//  BetweenLayer.h
//  SantaSleighPacking
//
//  Created by Anton Logunov on 12/23/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//


#ifndef __SantaSleighPacking__BetweenLayer__
#define __SantaSleighPacking__BetweenLayer__

/*
#include "BaseLayer.h"

struct TailLayer : Layer {
    TailLayer() : naiveStrategy(NaiveStrategy(*this)), rectSetStrategy(RectSetStrategy(*this)) {} 
        
    void initValGrid() {
        valGr.fill(sz_z_max);
        for_each(begin, begin+n, [&](Pr& p) {
            for (unsigned y = p.y; y < p.y+p.sz_y; y++) {
                for (unsigned x = p.x; x < p.x+p.sz_x; x++) {
                    valGr(x, y) = p.z-1;
                }
            }
        });
    }
    
    void writeFreeVolumeTable() {
        FILE *file = fopen("free_volume.txt", "w");
        for (unsigned y = 1; y <= SZ; y++) {    
            for (unsigned x = 1; x <= SZ; x++) {
                fprintf(file, "%4u", valGr.entry(x, y));
            }
            fprintf(file, "\n");
        }
        fclose(file);
    }
    
    struct Packing {
        Packing(TailLayer& tailL) : tailL(tailL) {}
        TailLayer &tailL;
        virtual bool pack() = 0;
    };
    
    struct RectSetStrategy : Strategy {
        RectSetStrategy(TailLayer& tailL) : Strategy(tailL) {}
        vector<LocRect> rects;
        Grid<int> dN, dS;
        void initRects(unsigned val) {
            Grid<unsigned> &valGrid = tailL.valGr;
            rects.clear();
        }
        
        void checkLocRect(const LocRect& r, unsigned val) {
            Grid<unsigned> &valGrid = tailL.valGr;  
            for (unsigned y = r.y; y < r.y + r.sz_y; y++) {
                for (unsigned x = r.x; x < r.x + r.sz_x; x++) {
                    if (valGrid.entry(x, y) < val) {
                        for (unsigned y = r.y; y < r.y + r.sz_y; y++) {
                            for (unsigned x = r.x; x < r.x + r.sz_x; x++) {
                                printf("%d ", valGrid.entry(x, y) >= val);
                            }
                            printf("\n");
                        }
                        assert(false);
                    }
                }
            }
            
        }
        
        bool fill(Pr& p) {
            unsigned val = tailL.val;
            unsigned sz_x = p.sz_x, 
            sz_y = p.sz_y;
            initRects(val);
            sort(rects.begin(), rects.end(), Rect::areaLess);
            auto r_it = lower_bound(rects.begin(), rects.end(), Rect(sz_x, sz_y), Rect::areaLess);
            while (r_it != rects.end()) {
                if (r_it->canInsert(Rect(sz_x, sz_y))) {
                    p.x = r_it->x;
                    p.y = r_it->y;
                    p.z = 1+val-p.sz_z;
                    return true;
                }
                if (r_it->canInsert(Rect(sz_y, sz_x))) {
                    p.x = r_it->x;
                    p.y = r_it->y;
                    p.z = 1+val-p.sz_z;
                    swap(p.sz_x, p.sz_y);
                    return true;
                }
                r_it++;
            }
            return false;
        }
    };
    
    
    struct NaiveStrategy : Strategy {
        NaiveStrategy(TailLayer& tailL) : Strategy(tailL) {}
        bool fill(Pr& p) {
            Grid<unsigned> &valGr = tailL.valGr;
            unsigned val = tailL.val;
            unsigned sz_x = p.sz_x, 
            sz_y = p.sz_y;
            for (unsigned y = 1; y <= SZ; y++) {
                for (unsigned x = 1; x <= SZ; x++) {
                    if (x+sz_x-1 > SZ || y+sz_y-1 > SZ) continue;
                    bool good = true;
                    // contour first
                    for (unsigned dx = x; dx < x+sz_x; dx++) {
                        if (!(valGr.entry(dx, y) >= val && valGr.entry(dx, y+sz_y-1) >= val)) {
                            good = false;
                            goto bad;
                        }
                    }
                    
                    for (unsigned dy = y; dy < y+sz_y; dy++) {
                        if (!(valGr.entry(x, dy) >= val && valGr.entry(x+sz_x-1, dy) >= val)) {
                            good = false;
                            goto bad;
                        }
                    }
                    
                    // inside next
                    for (unsigned dx = x+1; dx < x+sz_x-1; dx++) {
                        for (unsigned dy = y+1; dy < y+sz_y-1; dy++) {
                            if (valGr.entry(dx, dy) < val) {
                                good = false;
                                goto bad;
                            }
                        }
                    }
                bad:
                    if (good) {
                        // sz_s are already assigned 
                        p.x = x;
                        p.y = y;
                        p.z = 1+val-p.sz_z;
                        return true;
                    }
                }
            }
            return false;
        }
        
    };
    
    struct Best {
        Best(double performance, unsigned waste_sz_z, PR_IT end) 
            : performance(performance), waste_sz_z(waste_sz_z), end(end) {}
        double performance;
        unsigned waste_sz_z;
        PR_IT end;
    };

    bool fill(BaseLayer& baseL, PR_IT tail_begin, unsigned n_max) {
        sz_z_max = 1*baseL.sz_z_max/3;
        n = 0;
        putAbove(baseL);
        // n and begin taken from base layer
        
        initValGrid();
        //writeFreeVolumeTable();
        val = *max_element(valGr.grid.begin(), valGr.grid.end());
        
        unsigned base_prs_volume = volume(begin, n);
        Best best((double)base_prs_volume/(baseL.sz_z_max*SZ*SZ), sz_z_max-baseL.sz_z_max, tail_begin);
        unsigned cur_waste_sz_z = best.waste_sz_z;
        unsigned cur_prs_volume = base_prs_volume;
        
        // need to go in reverse order
        bool bounded = true;
        //unsigned n_tail = 0;
        for (unsigned n_tail = 0; n_tail <= n_max; n_tail++) {
            Pr &pr = *(tail_begin+n_tail);
            bounded = false;
            while (val >= pr.sz_z) {
                bounded = strategy.fill(pr);
                if (bounded) {
                    unsigned entry_val = val-pr.sz_z;
                    for (unsigned x = pr.x; x < pr.x+pr.sz_x; x++) {
                        for (unsigned y = pr.y; y < pr.y+pr.sz_y; y++) {
                            valGr.entry(x, y, entry_val);
                        }
                    }
                    if (cur_waste_sz_z > pr.z-1) cur_waste_sz_z = pr.z-1;
                    cur_prs_volume += pr.volume();
                    double performance = (double)cur_prs_volume/(SZ*SZ*(sz_z_max-cur_waste_sz_z)); 
                    if (performance > best.performance) {
                        best.performance = performance;
                        best.waste_sz_z = cur_waste_sz_z;
                        best.end = tail_begin+n_tail+1;
                    }
                    break;
                }
                else val--;
            }
            if (!bounded) break;
        }
        n = (unsigned)(best.end - begin);
        lower(best.waste_sz_z);
        // CHECKING HERE
        checkLayer(begin, n, 1, sz_z_max);
        return bounded;
    }

    Grid<unsigned> valGr;
    unsigned val;
    NaiveStrategy naiveStrategy;
    RectSetStrategy rectSetStrategy;

};
*/

#endif /* defined(__SantaSleighPacking__BetweenLayer__) */
