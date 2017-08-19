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
    
    MaxRectPacking() : nextMax(*this), 
                       nextMaxHuge(*this), 
                       nextMaxRandom(*this), 
                       topLeft(*this) {
        next = &nextMax;
        moveLess = &moveShortSide;
        isCandidate = &topLeft;
    }
    
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
        void init(Pr* pr, LocRect& rect, bool isRotated, Corner corner); 
        LocRect prLocRect() const;
        
        bool isIntersection(const Move& m) const {
            return prLocRect().isIntersection(m.prLocRect());
        }
    };
    
    struct MaxLocRect : LocRect {
        MaxLocRect(unsigned x, unsigned y, unsigned sz_x, unsigned sz_y, unsigned ind) 
            : LocRect(x, y, sz_x, sz_y), creatorIndex(ind) {}
        MaxLocRect(LocRect& r, unsigned ind) : LocRect(r), creatorIndex(ind) {}
        MaxLocRect() {}
        unsigned creatorIndex;
    };
    
    typedef vector<MaxLocRect>::iterator MLR_IT; 
    
    // chooses among candidate moves
    struct Next {
        MaxRectPacking& master;
        Next(MaxRectPacking& master) : master(master) {}
        void select(const Move& m) {
            master.solution.push_back(m);
            master.packingRects.erase(
              remove(master.packingRects.begin(), 
                     master.packingRects.end(), 
                     master.solution.back().pr));
        }
        // last one is success flag
        // if candidates is empty returns false else returns true.
        virtual void operator()() = 0;
    };
    
    // compares moves
    struct MoveLess {
        virtual bool operator()(const Move& m_1, const Move& m_2) const = 0;
    };
    
    // is candidate
    struct Candidate {
        MaxRectPacking& master;
        Candidate(MaxRectPacking& master) : master(master) {}
        virtual bool operator()(const Move& m) = 0;
    };
    
    struct CandidateTopLeft : Candidate {
        CandidateTopLeft(MaxRectPacking& master) : Candidate(master) {}
        bool operator()(const Move& m) {
            return m.corner == CORNER_TL;
        }
    };
        
    struct MoveShortSide : MoveLess {
        MoveShortSide() {}
        unsigned score(const Move& m) const;
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
            select(m);
        }
    };
    
    struct NextMaxHuge : Next {
        NextMaxHuge(MaxRectPacking& master) : Next(master) {}
        void operator()() {
            vector<Move>& cands = master.candidates; 
            function<bool(const Move&, const Move&)> less = [&](const Move& m_1, const Move& m_2) {
                return (*master.moveLess)(m_1, m_2) || 
                     (!(*master.moveLess)(m_2, m_1) && (m_1.pr->area() < m_2.pr->area()));
            };
            Move m = *max_element(cands.begin(), cands.end(), less);
            select(m);
        }
    };
    
    struct NextMaxRandom : Next {
        unsigned n_max;
        NextMaxRandom(MaxRectPacking& master) : Next(master) {
            n_max = 10;
        }
        void operator()() {
            vector<Move>& cands = master.candidates; 
            function<bool(const Move&, const Move&)> greater = [&](const Move& m_1, const Move& m_2) {
                // GREATER
                return (*master.moveLess)(m_2, m_1);
            };
            unsigned i = random()%min(n_max, (unsigned)cands.size());
            //sort(cands.begin(), cands.end(), greater);
            //nth_element(cands.begin(), cands.begin()+i, cands.end(), greater);
            Move m = *(cands.begin()+i);
            select(m);
        }
    };
    
    unsigned iterationIndex;
    
    Next *next;
    NextMax nextMax;
    NextMaxHuge nextMaxHuge;
    NextMaxRandom nextMaxRandom;
    
    MoveLess *moveLess;
    MoveShortSide moveShortSide;
    
    Candidate *isCandidate;
    CandidateTopLeft topLeft;
    
    
    vector<MaxLocRect> freeRects; 
    vector<Pr*> packingRects;
    vector<Move> solution;
    vector<Move> candidates;
    
    virtual void setNext(Next& next) {
        this->next = &next;
    }
    
    virtual void setMoveLess(MoveLess& moveLess) {
        this->moveLess = &moveLess;
    }
    
    virtual void setCandidate(Candidate& candidate) {
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
        unsigned p_index = (unsigned)solution.size()-1;
        int x, y, sz_x, sz_y;
        vector<MaxLocRect> rs;
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
                    rs.push_back(MaxLocRect(x, y, sz_x, sz_y, p_index));
                    //assert(r.hasInside(LocRect(x, y, sz_x, sz_y)));
                }
                // left
                x = r.x; sz_x = p_r.x - r.x; y = r.y; sz_y = r.sz_y;
                if (sz_x > 0) { 
                    rs.push_back(MaxLocRect(x, y, sz_x, sz_y, p_index));
                    //assert(r.hasInside(LocRect(x, y, sz_x, sz_y)));
                }
                // right 
                x = p_r.x + p_r.sz_x; sz_x = r.x + r.sz_x - x;
                y = r.y; sz_y = r.sz_y;
                if (sz_x > 0) {
                    rs.push_back(MaxLocRect(x, y, sz_x, sz_y, p_index));
                    //assert(r.hasInside(LocRect(x, y, sz_x, sz_y)));
                }
                // bottom
                x = r.x; sz_x = r.sz_x; 
                y = p_r.y + p_r.sz_y; sz_y = r.y + r.sz_y - y;
                if (sz_y > 0) {
                    rs.push_back(MaxLocRect(x, y, sz_x, sz_y, p_index));
                    //assert(r.hasInside(LocRect(x, y, sz_x, sz_y)));
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
        // go through all rects with r_it
        MLR_IT r_it = freeRects.end(), r_2_it;
        // first inner to remove
        MLR_IT e_it = freeRects.end(); 
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
            (*next)(); // clears packing rects from present
            updateFreeRects();
            removeSubMaxFreeRects();
        }
    }
    
    void init(PR_IT begin, unsigned n) {
        solution.clear();
        freeRects.clear();
        freeRects.push_back(MaxLocRect(1, 1, SZ, SZ, 0));
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
        if (packingRects.size() == 0) copySolution();
        return (unsigned)packingRects.size();
    } 
};



#endif /* defined(__SantaSleighPacking__MaxRectPacking__) */
