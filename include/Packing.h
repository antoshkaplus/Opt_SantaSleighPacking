//
//  Covering.h
//  SantaSleighPacking
//
//  Created by Anton Logunov on 12/26/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#ifndef __SantaSleighPacking__Covering__
#define __SantaSleighPacking__Covering__

#include "Help.h"

typedef vector<Pr*>::iterator PR_PT_IT;

struct Block : Rect {
    vector<Pr*> prs;
    Block() {}
    Block(unsigned sz_x, unsigned sz_y) : Rect(sz_x, sz_y) {}
    void init(unsigned sz_x, unsigned sz_y) {
        this->sz_x = sz_x; this->sz_y = sz_y;
    }
    
    void locate(Block& b, unsigned x, unsigned y) {
        prs.insert(prs.end(), b.prs.begin(), b.prs.end());
        for (Pr* p: b.prs) {
            p->x += x-1;
            p->y += y-1;
        }
    }
    
    void putBelow(Block& b) {}
    void putAbove(Block& b) {}
    void putLeft(Block& b) {}
    void putRight(Block& b) {}
    
    // i need to store presents here... with specified locations and sizes
};

struct Shelf : Block {
    Shelf(unsigned sz_x, unsigned sz_y) : Block(sz_x, sz_y) {}
    Shelf() {}

    // returns iterator begin to placed elements
    PR_PT_IT fill(PR_PT_IT begin, unsigned n) {
        prs.clear();
        unsigned sz_y_max = this->sz_y;
        for_each(begin, begin+n, [=](Pr* p) { 
            if (p->sz_y > sz_y_max || (p->sz_x <= sz_y_max && p->sz_x > p->sz_y)) {
                swap(p->sz_x, p->sz_y);
            }
        });
        // from biggest to smallest
        sort(begin, begin+n, Shelf::compPrPt);
        unsigned x = 1;
        PR_PT_IT pi = upper_bound(begin, begin+n, sz_y, Shelf::compUnsigned);
        if (pi == begin) return begin+n;
        while (true) {
            Pr &p = **(--pi);
            if (SZ >= x+p.sz_x-1) {
                p.x = x;
                p.y = 1;
                x += p.sz_x;
                prs.push_back(*pi);
            }
            else {
                pi++;
                break;
            }
            if (pi == begin) break;
        }
        return pi;
    }
    
    static bool compUnsigned(unsigned sz_y, Pr* p_1) {
        return sz_y < p_1->sz_y ;
    }
    static bool compPrPt(Pr* p_1, Pr* p_2) {
        return p_1->sz_y < p_2->sz_y;
    }
};


struct Packing {
    // returns count of unfilled elements
    virtual unsigned fillFixed(PR_IT begin, unsigned n) = 0;
};

struct CornerPacking : Packing {
    unsigned fillFixed(PR_IT begin, unsigned n) { return 1; }
};

struct LinePacking : Packing {
    vector<Pr*> prs;
    
    static unsigned maxMinSz(PR_PT_IT begin, unsigned n) {
        unsigned m, max = 0;
        for (auto b = begin; b != begin+n; b++) {
            m = min((*b)->sz_x, (*b)->sz_y);
            if (m > max) max = m;
        }
        return max;
    }

    void fillPrPts(PR_IT b, unsigned n) {
        prs.resize(n);
        for (unsigned i = 0; i < n; i++) {
            prs[i] = &(*(b+i));
        }
    }

    unsigned fillFixed(PR_IT begin, unsigned n) { 
        fillPrPts(begin, n);
        Block b(SZ, SZ);
        Shelf sh;
        unsigned sz_y, y = 1;
        while (prs.size() > 0) {
            sz_y = maxMinSz(prs.begin(), (unsigned)prs.size());
            if (sz_y > SZ-y+1) break;
            sh.init(SZ, sz_y);
            prs.erase(sh.fill(prs.begin(), (unsigned)prs.size()), prs.end());
            b.locate(sh, 1, y);
            y += sz_y;
        }
        return (unsigned)prs.size(); 
    }
};


#endif /* defined(__SantaSleighPacking__Covering__) */
