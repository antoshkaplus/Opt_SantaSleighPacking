//
//  MaxRectPacking.h
//  SantaSleighPacking
//
//  Created by Anton Logunov on 12/28/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#ifndef __SantaSleighPacking__MaxRectPacking__
#define __SantaSleighPacking__MaxRectPacking__

#include "Packing.h"

struct MaxRectPacking : Packing {
    MaxRectPacking() : nextMax(*this), nextMaxHuge(*this) {
        next = &nextMax;
        moveLess = &moveShortSide;
        isCandidate = &topLeft;
    }
    
    typedef vector<LocRect>::iterator LR_IT; 
    
    // chooses among candidated
    struct Next {
        MaxRectPacking& master;
        Next(MaxRectPacking& master) : master(master) {}
        // last one is success flag
        // if candidates is empty returns false else returns true.
        virtual void operator()() = 0;
    };
        
    enum Corner {
        CORNER_TL = 1,
        CORNER_TR = 2,
        CORNER_BL = 4,
        CORNER_BR = 8
    };
    
    struct Move {
        Pr* pr;
        // nuh copy the whole rect // later will delete it
        LocRect rect;
        bool isRotated;
        Corner corner;
        void init(Pr* pr, LocRect& rect, bool isRotated, Corner corner) {
            this->pr = pr;
            this->rect = rect;
            this->isRotated = isRotated;
            this->corner = corner;
        }
        LocRect prLocRect() {
            LocRect r;
            if (isRotated) {
                r.sz_x = pr->sz_y;
                r.sz_y = pr->sz_x;
            } else {
                r.sz_x = pr->sz_x;
                r.sz_y = pr->sz_y;
            }
            switch (corner) {
            case CORNER_BL:
                r.x = rect.x;
                r.y = rect.y+rect.sz_y - r.sz_y;
                break;
            case CORNER_BR: 
                r.x = rect.x+rect.sz_x - r.sz_x;
                r.y = rect.y+rect.sz_y - r.sz_y;
                break;
            case CORNER_TL:
                r.x = rect.x;
                r.y = rect.y;
                break;
            case CORNER_TR:
                r.x = rect.x+rect.sz_x - r.sz_x;
                r.y = rect.y;
                break;
            }
            return r;
        }
    };
        
    // compares moves
    struct MoveLess {
    
        virtual bool operator()(const Move& m_1, const Move& m_2) const = 0;
    };
    
    // is candidate
    struct Candidate {
        virtual bool operator()(const Move& m) = 0;
    };
    
    struct TopLeftCandidate : Candidate {
        bool operator()(const Move& m) {
            return m.corner == CORNER_TL;
        }
    };
    
    struct MoveShortSide : MoveLess {
        unsigned score(const Move& m) const {
            return m.isRotated ? min(m.rect.sz_x - (*m.pr).sz_y, 
                                     m.rect.sz_y - (*m.pr).sz_x) :
                                 min(m.rect.sz_x - (*m.pr).sz_x,
                                     m.rect.sz_y - (*m.pr).sz_y);
        }
        bool operator()(const Move& m_1, const Move& m_2) const {
            return score(m_1) > score(m_2);
        }
    }; 
    
    struct NextMax : Next {
        NextMax(MaxRectPacking& master) : Next(master) {}
        void operator()() {
            vector<Move>& cands = master.candidates; 
            function<bool(const Move&, const Move&)> less = [&](const Move& m_1, const Move& m_2) {
                return (*master.moveLess)(m_1, m_2);
            };
            Move m = *max_element(cands.begin(), cands.end(), less);
            master.solution.push_back(m);
            master.packingRects.erase(
                remove(master.packingRects.begin(), 
                       master.packingRects.end(), 
                       master.solution.back().pr));
        }
    };
    
    struct NextMaxHuge : Next {
        NextMaxHuge(MaxRectPacking& master) : Next(master) {}
        void operator()() {
            vector<Move>& cands = master.candidates; 
            function<bool(const Move&, const Move&)> less = [&](const Move& m_1, const Move& m_2) {
                return (*master.moveLess)(m_1, m_2) || 
                     (!(*master.moveLess)(m_2, m_1) && m_1.pr->area() < m_2.pr->area());
            };
            Move m = *max_element(cands.begin(), cands.end(), less);
            master.solution.push_back(m);
            master.packingRects.erase(
                remove(master.packingRects.begin(), 
                     master.packingRects.end(), 
                     master.solution.back().pr));
        }
    };
    
    Next *next;
    NextMax nextMax;
    NextMaxHuge nextMaxHuge;
    
    MoveLess *moveLess;
    MoveShortSide moveShortSide;
    
    Candidate *isCandidate;
    TopLeftCandidate topLeft;
    
    vector<LocRect> freeRects; 
    vector<Pr*> packingRects;
    /// present, where did put
    vector<Move> solution;
    vector<Move> candidates;
    
    void setNext(Next& next) {
        this->next = &next;
    }
    
    void setMoveLess(MoveLess& moveLess) {
        this->moveLess = &moveLess;
    }
    
    void setCandidate(Candidate& candidate) {
        this->isCandidate = &candidate;
    }
    
    void selectCandidates() {
        Move m;
        array<Corner, 4> cs = {CORNER_BL, CORNER_BR, CORNER_TL, CORNER_TR};
        candidates.clear();
        for (auto p_it = packingRects.begin(); p_it != packingRects.end(); p_it++) {
            for (auto r_it = freeRects.begin(); r_it != freeRects.end(); r_it++) {
                if (r_it->canInsert(**p_it)) {
                    for (Corner c : cs) {
                        m.init(*p_it, *r_it, false, c);
                        if ((*isCandidate)(m)) candidates.push_back(m);
                    }
                } 
                if (r_it->canInsertRotated(**p_it)) {  
                    for (Corner c : cs) {
                        m.init(*p_it, *r_it, true, c);
                        if ((*isCandidate)(m)) candidates.push_back(m);
                    }
                }
            }
        }
    }
    
    // probably some rects devided because of ...
    void updateFreeRects() {
        if (freeRects.size() < 1) return;
        LocRect p_r(solution.back().prLocRect());
        int x, y, sz_x, sz_y;
        vector<LocRect> rs;
        // go through all rects with it
        auto r_it = freeRects.end();
        // first intersected
        auto e_it = freeRects.end(); 
        while (r_it-- != freeRects.begin()) {
            LocRect &r = *r_it;
            if (r.isIntersection(p_r)) {
                // top
                x = r.x; sz_x = r.sz_x; y = r.y; sz_y = p_r.y - r.y;
                if (sz_y > 0) {
                    rs.push_back(LocRect(x, y, sz_x, sz_y));
                    assert(r.hasInside(LocRect(x, y, sz_x, sz_y)));
                }
                // left
                x = r.x; sz_x = p_r.x - r.x; y = r.y; sz_y = r.sz_y;
                if (sz_x > 0) { 
                    rs.push_back(LocRect(x, y, sz_x, sz_y));
                    assert(r.hasInside(LocRect(x, y, sz_x, sz_y)));
                }
                // right 
                x = p_r.x + p_r.sz_x; sz_x = r.x + r.sz_x - x;
                y = r.y; sz_y = r.sz_y;
                if (sz_x > 0) {
                    rs.push_back(LocRect(x, y, sz_x, sz_y));
                    assert(r.hasInside(LocRect(x, y, sz_x, sz_y)));
                }
                // bottom
                x = r.x; sz_x = r.sz_x; 
                y = p_r.y + p_r.sz_y; sz_y = r.y + r.sz_y - y;
                if (sz_y > 0) {
                    rs.push_back(LocRect(x, y, sz_x, sz_y));
                    assert(r.hasInside(LocRect(x, y, sz_x, sz_y)));
                }
                e_it--;
                swap(*r_it, *e_it);
            }
        }
        freeRects.erase(e_it, freeRects.end());
        freeRects.insert(freeRects.end(), rs.begin(), rs.end());
    }
    void removeSubMaxFreeRects() {
        if (freeRects.size() < 2) return;
        sort(freeRects.begin(), freeRects.end(), Rect::areaLess);
        // go through all rects with it
        LR_IT r_it = freeRects.end(), r_2_it;
        // first inner to remove
        LR_IT e_it = freeRects.end(); 
        while (r_it-- != freeRects.begin()) {
            r_2_it = r_it;
            while (++r_2_it != e_it) {
                if (r_2_it->hasInside(*r_it)) {
                    swap(*r_it, *(--e_it));
                    break; 
                }
            }
        }
        freeRects.erase(e_it, freeRects.end());
    }
    
    void solve() {
        while (packingRects.size() > 0) {
            selectCandidates();
            if (candidates.size() == 0) break;
            (*next)();
            updateFreeRects();
            // clear packing rects from present
            removeSubMaxFreeRects();
        }
        copySolution();
    }
    
    void init(PR_IT begin, unsigned n) {
        solution.clear();
        freeRects.clear();
        freeRects.push_back(LocRect(1, 1, SZ, SZ));
        packingRects.resize(n);
        for (unsigned i = 0; i < n; i++) {
            packingRects[i] = &(*(begin+i));
        }
    }
    
    void copySolution() {
        for (auto s = solution.begin(); s != solution.end(); s++) {
            s->pr->fromLocRect(s->prLocRect());
        }
    }
    
    unsigned fillFixed(PR_IT begin, unsigned n) {
        init(begin, n);
        solve();
        return (unsigned)packingRects.size();
    } 
    
    
};



#endif /* defined(__SantaSleighPacking__MaxRectPacking__) */
