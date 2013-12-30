//
//  Gr.h
//  SantaSleighPacking
//
//  Created by Anton Logunov on 12/11/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//


/*
struct C {
    C(unsigned x, unsigned y, int dx, int dy) : x(x), y(y), dx(dx), dy(dy) {}
    unsigned x, y;
    int dx, dy;  
};
*/

#ifndef SantaSleighPacking_Gr_h
#define SantaSleighPacking_Gr_h

#include "Help.h"

struct Gr {
    bool arr[SZ*SZ];
    //list<C> cs;
    Gr() {
        /*
        cs = {C( 1, 1,  1, 1), C( 1, SZ,  1, -1), 
              C(SZ, 1, -1, 1), C(SZ, SZ, -1, -1)};
        */
        memset(arr, 0, sizeof(bool)*SZ*SZ);
    }
    /*
    void fill(vector<Pr>& prs, unsigned i_start, unsigned n) {
        for (unsigned i = i_start; i < i_start+n; i++) {
            Pr &p = prs[i];
            for (unsigned x = p.loc.x_1; x <= p.loc.x_2; x++) {
                for (unsigned y = p.loc.y_1; y <= p.loc.y_2; y++) {
                    assert(!entry(x, y));
                    setEntry(x, y);
                }
            }
        }
    }
    */
    bool entry(unsigned x, unsigned y) {
        return arr[(y-1)*SZ + (x-1)];
    }
    void setEntry(unsigned x, unsigned y) {
        arr[(y-1)*SZ + (x-1)] = true;
    }
    /*
    int placeScore(Pr& pr, C& c) {
        int x_1, x_2, y_1, y_2;
        unsigned sz_x = pr.sz_x, sz_y = pr.sz_y, 
        score = 0; 
        (c.dx > 0 ? tie(x_1, x_2) : tie(x_2, x_1)) = make_tuple(c.x, c.x + (sz_x-1)*c.dx);
        (c.dy > 0 ? tie(y_1, y_2) : tie(y_2, y_1)) = make_tuple(c.y, c.y + (sz_y-1)*c.dy);
        if (x_1 < 1 || x_2 > SZ || y_1 < 1 || y_2 > SZ) return -1;
        for (unsigned x = x_1; x <= x_2; x++) {
            if (entry(x, y_1) || entry(x, y_2)) {
                return -1;
            }
            if (y_1-1 == 0 || entry(x, y_1-1)) ++score;
            if (y_2+1 == SZ || entry(x, y_2+1)) ++score;
        }
        for (unsigned y = y_1; y <= y_2; y++) {
            if (entry(x_1, y) || entry(x_2, y)) {
                return -1;
            }
            if (x_1-1 == 0 || entry(x_1-1, y)) ++score;
            if (x_2+1 == SZ || entry(x_2+1, y)) ++score;
        }
        return score;
    }
    void place(Pr& pr, C& c) {
        unsigned &x_1 = pr.loc.x_1, 
        &x_2 = pr.loc.x_2, 
        &y_1 = pr.loc.y_1, 
        &y_2 = pr.loc.y_2;
        unsigned sz_x = pr.sz_x, sz_y = pr.sz_y;
        (c.dx > 0 ? tie(x_1, x_2) : tie(x_2, x_1)) = make_tuple(c.x, c.x + (sz_x-1)*c.dx);
        (c.dy > 0 ? tie(y_1, y_2) : tie(y_2, y_1)) = make_tuple(c.y, c.y + (sz_y-1)*c.dy);
        assert(x_1 >= 1 && y_1 >= 1 && x_2 <= SZ && y_2 <= SZ);
        for (unsigned x = x_1; x <= x_2; x++) {
            setEntry(x, y_1);
            setEntry(x, y_2);
        }
        if (y_1-1 != 0) {
            for (unsigned x = x_1; x <= x_2; x++) {
                if ((x-1 == 0 || entry(x-1, y_1-1)) && !entry(x, y_1-1)) {
                    cs.push_back(C(x, y_1-1, -1, 1));
                }
            }
            for (unsigned x = x_2; x >= x_1; x--) {
                if ((x+1 == SZ+1 || entry(x+1, y_1-1)) && !entry(x, y_1-1)) {
                    cs.push_back(C(x, y_1-1, -1, -1));
                }
            }
        }
        if (y_2+1 != SZ+1) {
            for (unsigned x = x_1; x <= x_2; x++) {
                if ((x-1 == 0 || entry(x-1, y_2+1)) && !entry(x, y_2+1)) {
                    cs.push_back(C(x, y_2+1, 1, 1));
                }
            }
            for (unsigned x = x_2; x >= x_1; x--) {
                if ((x+1 == SZ+1 || entry(x+1, y_2+1)) && !entry(x, y_2+1)) {
                    cs.push_back(C(x, y_2+1, 1, -1));
                }
            }
        }
        
        for (unsigned y = y_1; y <= y_2; y++) {
            setEntry(x_1, y);
            setEntry(x_2, y);
        }
        if (x_1-1 != 0) {
            for (unsigned y = y_1; y <= y_2; y++) {
                if ((y-1 == 0 || entry(x_1-1, y-1)) && !entry(x_1-1, y)) {
                    cs.push_back(C(x_1-1, y, 1, -1));
                }
            }
            for (unsigned y = y_2; y >= y_1; y--) {
                if ((y+1 == SZ+1 || entry(x_1-1, y+1)) && !entry(x_1-1, y)) {
                    cs.push_back(C(x_1-1, y, -1, -1));
                }
            }
        }
        if (x_2+1 != SZ+1) {
            for (unsigned y = y_1; y <= y_2; y++) {
                if ((y-1 == 0 || entry(x_2+1, y-1)) && !entry(x_2+1, y)) {
                    cs.push_back(C(x_2+1, y, 1, 1));
                }
            }
            for (unsigned y = y_2; y >= y_1; y--) {
                if ((y+1 == SZ+1 || entry(x_2+1, y+1)) && !entry(x_2+1, y)) {
                    cs.push_back(C(x_2+1, y, -1, 1));
                }
            }
        }
        typedef list<C>::iterator it;
        for (it c = cs.begin(); c != cs.end(); c++) {
            assert(c->x >= 1 && c->x <= SZ && c->y >= 1 && c->y <= SZ);
        }
        
    }
    bool place(Pr& pr) {
        typedef list<C>::iterator it;
        vector<it> max_cs;
        unsigned max_s = 0;
        int s;
        for (it c = cs.begin(), c_2; c != cs.end(); c++) {
            if (entry(c->x, c->y)) continue;
            // corner exists???
            s = placeScore(pr, *c);
            if (s < 0) continue;
            if (s > max_s) {
                max_cs = {c};
                max_s = s;
            } else if (s == max_s) {
                // need c as iterator probably
                max_cs.push_back(c);
            }
        }
        if (max_cs.size() == 0) return false;
        it c = max_cs[random()%max_cs.size()];
        place(pr, *c);
        cs.erase(c);
        return true;
    }
    */
};



#endif
