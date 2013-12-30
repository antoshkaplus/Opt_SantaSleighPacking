//
//  Sh.h
//  SantaSleighPacking
//
//  Created by Anton Logunov on 12/14/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#ifndef __SantaSleighPacking__Sh__
#define __SantaSleighPacking__Sh__

#include "Help.h"


struct Sh {
    unsigned y_1, y_sz_max;
    vector<Pr*> prs;
    array<unsigned, SZ> h;
    Sh(){}
    Sh(unsigned y_1, unsigned y_sz_max) : y_1(y_1), y_sz_max(y_sz_max) {
        std::fill(h.begin(), h.end(), y_sz_max);
    }
    void fillShelf(vector<Pr*>& prs) {
        // both should be ok
        unsigned y_sz_max = this->y_sz_max;
        for_each(prs.begin(), prs.end(), [&y_sz_max](Pr* p) { 
            if (p->sz_y > y_sz_max || (p->sz_x <= y_sz_max && p->sz_x > p->sz_y)) {
                swap(p->sz_x, p->sz_y);
            }
        });
        // from biggest to smallest
        sort(prs.begin(), prs.end(), [](Pr* p_1, Pr* p_2) {
            return p_1->sz_y > p_2->sz_y;
        });
        
        unsigned x_1 = 1, x_2, y_2;
        for (auto pi = prs.begin(); pi != prs.end();) {
            Pr &p = **pi;
            x_2 = x_1+p.sz_x-1;
            y_2 = y_1+p.sz_y-1;
            if (SZ >= x_2) {
                p.x = x_1;
                p.y = y_1;
                // wanna keep info about shelf load ???
                this->prs.push_back(*pi);
                for (unsigned x = x_1-1; x <= x_2-1; x++) {
                    h[x] = y_sz_max-p.sz_y;
                }
                x_1 += p.sz_x;
                pi = prs.erase(pi);
            }
            else {
                pi++;
            }
        }
    }
};


struct JoSh {
    unsigned y_1, y_sz_max;
    array<unsigned, SZ> h;
    vector<Pr*> prs;
    JoSh(Sh& sh_top, Sh& sh_bot) {
        array<unsigned, SZ> &h_top = sh_top.h;
        array<unsigned, SZ> &h_bot = sh_bot.h;
        for (unsigned i = 0; i < SZ; i++) {
            h[i] = h_top[i] + h_bot[SZ-i-1];
        }
        unsigned d = *min_element(h.begin(), h.end());
        y_sz_max = sh_top.y_sz_max + sh_bot.y_sz_max; // -d later
        y_1 = sh_top.y_1;
        for (auto& pr : sh_bot.prs) {
            pr->x = SZ - pr->x - pr->sz_x + 2;
            pr->y = y_1+y_sz_max-pr->sz_y -d;
        }
        y_sz_max -= d;
        prs.resize(sh_top.prs.size() + sh_bot.prs.size());
        copy(sh_bot.prs.begin(), sh_bot.prs.end(), copy(sh_top.prs.begin(), sh_top.prs.end(), prs.begin()));
    }
};


unsigned fillFixed(PR_IT begin, unsigned n) {
    vector<Pr*> prsPs(n);
    for (unsigned i = 0; i < n; i++) {
        prsPs[i] = &(*(begin++));
    }
    unsigned y = 1, i = 0;
    Sh prev;
    while (prsPs.size() > 0) {
        Pr p = **max_element(prsPs.begin(), prsPs.end(), [](Pr* p_1, Pr* p_2) {
            return min(p_1->sz_x, p_1->sz_y) < min(p_2->sz_x, p_2->sz_y);
        });
        unsigned sz_y_max = min(p.sz_x, p.sz_y);
        if (y + sz_y_max-1 > SZ) break;
        Sh sh(y, sz_y_max);
        sh.fillShelf(prsPs);
        
        if (i++%2 == 1) {
            JoSh joSh(prev, sh);
            y += joSh.y_sz_max - prev.y_sz_max;
        } else {
            prev = sh;
            y += sh.y_sz_max;
        }
    }
    return (unsigned)prsPs.size();
}

unsigned fillShelfNextFit(PR_IT begin, unsigned n_max) {
    unsigned x_1 = 1, x_2, y_1 = 1, y_2, y_max = 0, sz_x, sz_y, k = 0;
    while (n_max--) {
        sz_x = begin->sz_x;
        sz_y = begin->sz_y;
        try_again:
        x_2 = x_1 + sz_x-1;
        y_2 = y_1 + sz_y-1;
        if (x_2 > SZ) {
            x_1 = 1;
            y_1 += y_max;
            y_max = 0;
            goto try_again;
        }
        if (y_2 > SZ) {
            break;
        }
        if (sz_y > y_max) y_max = sz_y;
        begin->x = x_1;
        begin->y = y_1;
        x_1 = x_2 + 1;
        begin++;
        k++;
    }
    return k;
}




#endif /* defined(__SantaSleighPacking__Sh__) */
