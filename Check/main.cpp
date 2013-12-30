//
//  main.cpp
//  Check
//
//  Created by Anton Logunov on 12/12/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#include <iostream>
#include <numeric>
#include <list>

#include "Help.h"
#include "TailLayer.h"
#include "MaxRectsBinPack.h"


void initPrs2d(vector<Pr>& prs) {
    unsigned n = 9;
    prs.resize(n);
    vector<tuple<unsigned, unsigned>> ps = {{ 
        {500, 500}, {250, 250}, {250, 250}, 
        {250, 250}, {250, 250}, {250, 500}, 
        {500, 250}, {250, 500}, {500, 250} }};
    for (unsigned i = 0; i < n; i++) {
        tie(prs[i].sz_x, prs[i].sz_y) = ps[i];
        prs[i].sz_z = 1000;
    }
}

/*
void checkGuillotine() {
       Guillotine g;
    printf("%s\n", g.fillFixed(prs, 0, (unsigned)prs.size()) ? "true" : "false");
    
    FILE *file = fopen("check_guillotine.txt", "SZ");
    fprintf(file, "%u,%u\n", SZ, SZ);
    for (Pr& p: prs) {
        fprintf(file, "%u,%u,%u,%u\n", p.loc.x_1, p.loc.y_1, p.loc.x_2, p.loc.y_2);
    }
    fclose(file);

}
*/

void checkCovering() {
    vector<Pr> prs;
    initPrs2d(prs);
    LineCovering covering;
    printf("not covered: %d\n", covering.fillFixed(prs.begin(), (unsigned)prs.size()));
    writePrs2D(prs.begin(), (unsigned)prs.size());
}


void checkTailLayer() {
    const unsigned SZ = 10;
    bool arr[SZ][SZ];
    for (unsigned y = 0; y < SZ; y++) {
        for (unsigned x = 0; x < SZ; x++) {
            arr[x][y] = random()%2;
        }
    }
    
    vector<LocRect> closedR;
    
    int dN[SZ][SZ] ;
    for (int x = 0; x < SZ; x++) {
        dN[x][0] = arr[x][0] ? 0 : -1;
    }
    for (int y = 1; y < SZ; ++y) {
        for (int x = 0 ; x < SZ; x++) {
            if (!arr[x][y]) dN[x][y] = -1;
            else dN[x][y] = dN[x][y-1] + 1 ;
        }
    }
    int dS[SZ][SZ];
    for (int x = 0; x < SZ ; ++ x ) {
        dS[x][SZ-1] = arr[x][SZ-1] ? 0 : -1;
    }
    for (int y = SZ-2; y >= 0; y--) {
        for (int x = 0; x < SZ ; x++) {
            if (!arr[x][y]) dS[x][y] = -1;
            else dS [x][y] = dS[x][y+1] + 1 ;
        }
    }
    for (int x = SZ-1; x >= 0; x--) {
        int maxS = SZ;
        for (int y = SZ-1; y >= 0; y--) {
            maxS++;
            if (arr[x][y] && (x == 0 || !arr[x-1][y])) {
                int N = dN[x][y];
                int S = dS[x][y];
                int width = 1;
                while (x + width < SZ && arr[x + width][y]) {
                    int nextN = dN[x+width][y];
                    int nextS = dS[x+width][y];
                    if ((nextN < N) || (nextS < S)) {
                        if (S < maxS) closedR.push_back(LocRect(x, y-N, width, N+S+1));
                        if (nextN < N) N = nextN;
                        if (nextS < S) S = nextS;
                    }
                    width++;
                }
                if (S < maxS) closedR.push_back(LocRect(x, y-N, width, N+S+1));
                maxS = 0 ;
            }
        }
    }
                    
    FILE *file = fopen("tail_layer.txt", "w");
    for (unsigned y = 0; y < SZ; y++) {
        for (unsigned x = 0; x < SZ; x++) {
            fprintf(file, "%d ", (int)arr[y][x]);
        }
        fprintf(file, "\n");
    }
    printf("rectangles:");
    for (unsigned i = 0; i < closedR.size(); i++) {
        LocRect &r = closedR[i];
        fprintf(file, "%u, %u, %u, %u\n", r.x, r.y, r.sz_x, r.sz_y);
    }
    
}

int main(int argc, const char * argv[])
{
    vector<Pr> prs(N);
    readPrsSzs(prs.begin(), N);
    rbp::MaxRectsBinPack b(SZ, SZ);
    vector<rbp::RectSize> rects;
    unsigned n = 270;
    vector<rbp::Rect> dst(n);
    for (unsigned i = 0; i < n; i++) {
        rbp::RectSize r;
        r.width = prs[i].sz_x;
        r.height = prs[i].sz_y;
        rects.push_back(r);
    }
    b.Insert(rects, dst, rbp::MaxRectsBinPack::RectBestAreaFit);
    printf("%f\n", b.Occupancy());
    for (unsigned i = 0; i < n; i++) {
        prs[i].x = dst[i].x+1;
        prs[i].y = dst[i].y+1;
        prs[i].sz_x = dst[i].width;
        prs[i].sz_y = dst[i].height;
    }
    for (unsigned i = 0; i < n; i++) {
        writePrs2D(prs.begin(), n);
    }
}

