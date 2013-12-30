//
//  TabuSearchMaxRectPacking.h
//  SantaSleighPacking
//
//  Created by Anton Logunov on 12/28/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#ifndef __SantaSleighPacking__TabuSearchMaxRectPacking__
#define __SantaSleighPacking__TabuSearchMaxRectPacking__

#include <unordered_map>
#include <fstream>

#include "MaxRectPacking.h"

struct TabuSearchMaxRectPacking : MaxRectPacking { 
    TabuSearchMaxRectPacking() : log(*this) {}

    MaxRectFinder maxRectFinder;
    // need init this two to get key right
    unsigned presentIdIndent;
    unsigned max_sz;
    // init this value after getting solution
    unsigned filledArea;
    bool bestFilledAreaChanged;
    //unsigned unimprovingIterationsCount;
    unsigned iterationIndex;
    unsigned allowableAdjMaxRectCount;
    bool logEnabled;
    
    vector<unsigned> adjMaxRectCount;
    struct Value {
        Value() {}
        Value(unsigned area, unsigned aspiration) : area(area), aspiration(aspiration) {}
        unsigned area; // filled area
        unsigned aspiration;
    };
    unordered_map<unsigned long, Value> tabuList;
    typedef unordered_map<unsigned long, Value>::iterator TL_IT;
    
    vector<TL_IT> remIts;
    void updateTabuList() {
        remIts.clear();
        for (auto t = tabuList.begin(); t != tabuList.end(); t++) {
            if (t->second.area > filledArea) {
                t->second.aspiration--;
                if (t->second.aspiration == 0) remIts.push_back(t);
            }
        }
        for (auto t = remIts.begin(); t != remIts.end(); t++) {
            tabuList.erase(*t);
        }
        for (unsigned i = 0; i < solution.size(); i++) {
            if (adjMaxRectCount[i] > 0) {
                tabuList[key(solution[i])] = Value(filledArea, adjMaxRectCount[i]*2);
            }
        }
    }
    
    unsigned long key(const Move& m) {
        return (m.pr->id-presentIdIndent)*max_sz*max_sz*4 + max_sz*m.rect.sz_y*4 + m.rect.sz_x*4 + m.corner;
    }
    
    Grid<bool> filledGrid;
    void assignNeighborhood() {
        filledGrid.fill(false);
        for (unsigned i = 0; i < solution.size();) {
            if (adjMaxRectCount[i] <= allowableAdjMaxRectCount) {
                Pr &p = *solution[i].pr;
                for (unsigned x = p.x; x < p.x+p.sz_x; x++) {
                    for (unsigned y = p.y; y < p.y+p.sz_y; y++) {
                        filledGrid(x, y) = true;
                    }
                }
                i++;
            }
            else {
                packingRects.push_back(solution[i].pr);
                solution.erase(solution.begin()+i);
            }
        }
        maxRectFinder.find(freeRects, filledGrid);
    }
    
    void countAdjMaxRects() {
        adjMaxRectCount.resize(solution.size(), 0);
        unsigned i = 0;
        for (auto s = solution.begin(); s != solution.end(); s++, i++) {
            for (auto r = freeRects.begin(); r != freeRects.end(); r++) {
                if (LocRect::areAdjucent(s->pr->toLocRect(), *r)) {
                    adjMaxRectCount[i]++;
                }
            }
        }
    }
    
    void computeFilledArea() {
        filledArea = 0;
        for (auto s = solution.begin(); s != solution.end(); s++) {
            filledArea += s->pr->area();
        }
    }
    struct Log {
        TabuSearchMaxRectPacking& master;
        Log(TabuSearchMaxRectPacking& master) : master(master) {}
        ofstream log;
        void open(unsigned i) {
            log.open("tabu_search_" + to_string(i) + ".csv");
            log << "iterationIndex,filledArea,isNewBestArea,allowableMaxRectCount,totalMaxRectCount" << endl;
        }
        void write() {
            log << master.iterationIndex << ","  
                << master.filledArea << ","
                << master.bestFilledAreaChanged << ","
                << master.allowableAdjMaxRectCount << ","
                << master.freeRects.size() << endl;
        }
        void close() {
            log.close();
        }
        
    };
    // CANT USE RIGHT NOW
    Log log;
    unsigned fillFixed(PR_IT begin, unsigned n) {
        unsigned bestFilledArea = 0;
        presentIdIndent = begin->id;
        iterationIndex = 0;
        if (logEnabled) log.open(presentIdIndent);
        max_sz = maxSz(begin, n);
        init(begin, n);
        unsigned minUnplacedPresentsCount = n;
        allowableAdjMaxRectCount = 2;
        array<unsigned, 3> iterationCountPerStage = {5, 200, 1}; 
        array<unsigned, 3> stageIterationCount = iterationCountPerStage;
        //fill(stageIterationCount.begin(), stageIterationCount.end(), iterationCountPerStage);
        while (true) {
            solve();
            
            // gather stats
            if (packingRects.size() < minUnplacedPresentsCount) {
                minUnplacedPresentsCount = (unsigned)packingRects.size();
            }
            computeFilledArea();
            if (filledArea > bestFilledArea) {
                bestFilledArea = filledArea;
                bestFilledAreaChanged = true;
            }
            else {
                bestFilledAreaChanged = false;
            }
            /**
            if (--stageIterationCount[allowableAdjMaxRectCount] == 0) {
                
                //stageIterationCount[allowableAdjMaxRectCount] = iterationCountPerStage;
                while (stageIterationCount[allowableAdjMaxRectCount] == 0) {
                    stageIterationCount[allowableAdjMaxRectCount] = iterationCountPerStage[allowableAdjMaxRectCount]-1;
                    allowableAdjMaxRectCount--;
                }
                if (stageIterationCount[0] == 0) break;
                stageIterationCount[allowableAdjMaxRectCount]--;
                /*
                for (unsigned i = allowableAdjMaxRectCount; i < 3; i++) {
                    stageIterationCount[i]--;
                }
                *
            }
            */
            countAdjMaxRects();
            // dont go out before logged // need to know allowableAdjMaxRectCount
            if (logEnabled) log.write();

            if (minUnplacedPresentsCount == 0) {
                break;
            }
            updateTabuList();
            assignNeighborhood();
            iterationIndex++;
            /*
            if (allowableAdjMaxRectCount < 2) {
                allowableAdjMaxRectCount = 2;
            }
            */
        }
        if (logEnabled) log.close();
        writePrs2D(begin, n);
        return minUnplacedPresentsCount;
    }
};



#endif /* defined(__SantaSleighPacking__TabuSearchMaxRectPacking__) */
