//
//  main.cpp
//  app
//
//  Created by Anton Logunov on 12/12/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#include <iostream>
#include <numeric>
#include <list>

#include "Help.h"
#include "BaseLayer.h"
#include "TailLayer.h"


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

int main(int argc, const char * argv[])
{
    FILE *file = fopen("tabu_log/r.txt", "w"); 
    fclose(file);
    mk
    vector<Pr> prs(N);
    readPrsSzs(prs.begin(), N);
    cout << BaseLayer::RecursiveSearch::estimatePerfectHeight(prs.begin(), N);
    
    
    /*
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
    */
}

