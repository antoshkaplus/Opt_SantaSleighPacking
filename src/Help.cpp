//
//  Help.c
//  include
//
//  Created by Anton Logunov on 1/1/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#include "../SantaSleighPacking/Help.h"

void readPrsSzs(PR_IT begin, unsigned n) {
    FILE *file = fopen("presents.csv", "r");
    assert(file!=nullptr);
    char str[100];
    fscanf(file, "%s", str);
    for (unsigned i = 0; i < n; i++) {
        Pr &pr = *(begin++);
        fscanf(file, "%u,%u,%u,%u", &pr.id, &pr.sz_x, &pr.sz_y, &pr.sz_z);
    };
    fclose(file);
}

void writePrsLocs(PR_C_IT begin, unsigned n) {
    FILE *file = fopen("presents.txt", "w");
    assert(file!=nullptr);
    fprintf(file, "PresentId,x1,y1,z1,x2,y2,z2,x3,y3,z3,x4,y4,z4,x5,y5,z5,x6,y6,z6,x7,y7,z7,x8,y8,z8\n");
    for (unsigned i = 0; i < n; i++) {
        const Pr &pr = *(begin++);
        unsigned x_1 = pr.x, x_2 = x_1+pr.sz_x-1, 
        y_1 = pr.y, y_2 = y_1+pr.sz_y-1, 
        z_1 = pr.z, z_2 = z_1+pr.sz_z-1;
        array<SpaceLoc, 8> ps = {SpaceLoc(x_1, y_1, z_1), SpaceLoc(x_2, y_2, z_2), SpaceLoc(x_1, y_2, z_1), 
            SpaceLoc(x_1, y_2, z_2), SpaceLoc(x_2, y_1, z_1), SpaceLoc(x_2, y_2, z_1),
            SpaceLoc(x_1, y_1, z_2), SpaceLoc(x_2, y_1, z_2)}; 
        
        fprintf(file, "%u", pr.id);
        for_each(ps.begin(), ps.end(), [file](const SpaceLoc& p) {
            fprintf(file, ",%u,%u,%u", p.x, p.y, p.z);
        });
        fprintf(file, "\n");
    }
    fclose(file);
}

void writePrsLayer(PR_C_IT begin, unsigned n) {
    string file_name = "layer-" + to_string(begin->id) + ".txt";
    FILE *file = fopen(file_name.c_str(), "w");
    assert(file!=nullptr);
    for (auto b = begin; b != begin+n; b++) {
        const Pr &p = *b;
        fprintf(file, "%u, %u, %u, %u, %u, %u, %u\n", p.id, p.x, p.y, p.z, p.sz_x, p.sz_y, p.sz_z);
    }
    fclose(file);
}

void writePrs2D(PR_C_IT begin, unsigned n) {
    FILE *file = fopen("2d.txt", "w");
    assert(file!=nullptr);
    fprintf(file, "%u,%u\n", SZ, SZ);
    for (auto p_it = begin; p_it != begin+n; p_it++) {
        fprintf(file, "%u,%u,%u,%u\n", p_it->x, p_it->y, p_it->x+p_it->sz_x-1, p_it->y+p_it->sz_y-1);
    }
    fclose(file);
}


void checkLayer(PR_C_IT begin, unsigned n, unsigned z_min, unsigned z_max) {
    for (PR_C_IT b = begin; b != begin+n; b++) {
        const Pr &p = *b;
        if (!(p.x >= 1 && p.x+p.sz_x-1 <= SZ && 
              p.y >= 1 && p.y+p.sz_y-1 <= SZ && 
              p.z >= z_min && p.z+p.sz_z-1 <= z_max)) {
            printf("out of bounds %u\n", begin->id);
            assert(0);
        }
        for (PR_C_IT e = b+1; e != begin+n; e++) {
            assert(!p.isIntersection(*e));
        }
    }
}

void collectSzs(vector<unsigned>& r, PR_C_IT b, unsigned n, unsigned min_sz) { 
    r.clear();
    auto e = b+n;
    while (b != e) {
        if (b->sz_x >= min_sz) r.push_back(b->sz_x);
        if (b->sz_y >= min_sz) r.push_back(b->sz_y);
        if (b->sz_z >= min_sz) r.push_back(b->sz_z);
        b++;
    }
    sort(r.begin(), r.end());
    r.erase(unique(r.begin(), r.end()), r.end());
}


unsigned maxSzZ(PR_C_IT b, unsigned n) {
    unsigned max_sz = 0;
    PR_C_IT e = b+n;
    while (b != e) {
        max_sz = max(max_sz, (b++)->sz_z);
    }
    return max_sz;
}

unsigned maxSz(Pr& p) {
    return max({p.sz_x, p.sz_y, p.sz_z});
}
unsigned minSz(Pr& p) {
    return min({p.sz_x, p.sz_y, p.sz_z});
}
unsigned minSz(Pr* p) {
    return min({p->sz_x, p->sz_y, p->sz_z});
}


unsigned maxSz(PR_IT b, unsigned n) {
    unsigned m = 0;
    auto e = b + n;
    while (b != e) {
        m = max(m, maxSz(*(b++)));
    }
    return m; 
}


unsigned area(PR_C_IT b, unsigned n) {
    unsigned a = 0;
    PR_C_IT e = b+n;
    while (b != e) {
        a += (b++)->area();
    }
    return a;
}

u_long volume(PR_C_IT b, unsigned n) {
    u_long v = 0;
    PR_C_IT e = b+n;
    while (b != e) {
        v += (b++)->volume();
    }
    return v;
}

double performance(PR_C_IT b, unsigned n) {
    return (double)volume(b, n)/(maxSzZ(b, n)*SZ*SZ);
}


// returns number of elements bounded (prepare for plane placing)
unsigned boundSzZ(PR_IT begin, unsigned n_max, unsigned sz_z_max) {
    return boundSzZ(begin, n_max, sz_z_max, 1.);
}


// returns number of elements bounded (prepare for plane placing)
unsigned boundSzZ(PR_IT begin, unsigned n_max, unsigned sz_z_max, double performance) {
    array<unsigned, 3> arr, arrI = {0, 1, 2};
    function<bool(unsigned)> notFit = [&arr, &sz_z_max](unsigned i){ 
        return arr[i] > sz_z_max; 
    };
    function<bool(unsigned,unsigned)> comp = [&arr](unsigned i_1, unsigned i_2) {
        return arr[i_1] < arr[i_2];
    };
    unsigned area = 0, i;
    for (i = 0; i != n_max; i++) {
        Pr &p = *(begin+i);
        arr = {p.sz_x, p.sz_y, p.sz_z};
        // should work on that
        auto a_it = partition(arrI.begin(), arrI.end(), notFit);
        if (a_it == arrI.end()) break;
        unsigned i_a = *max_element(a_it, arrI.end(), comp);
        swap(arr[2], arr[i_a]);
        p.sz_x = arr[0];
        p.sz_y = arr[1];
        p.sz_z = arr[2];
        if ((area += p.area()) > performance*SZ*SZ) break;
    }
    return i;
}


