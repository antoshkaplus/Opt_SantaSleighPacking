//
//  MaxRectPacking.cpp
//  SantaSleighPacking
//
//  Created by Anton Logunov on 1/1/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#include "../SantaSleighPacking/MaxRectPacking.h"

void MaxRectPacking::Move::init(Pr* pr, LocRect& rect, bool isRotated, Corner corner) {
    this->pr = pr;
    this->rect = rect;
    this->isRotated = isRotated;
    this->corner = corner;
}

LocRect MaxRectPacking::Move::prLocRect() const {
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

unsigned MaxRectPacking::MoveShortSide::score(const Move& m) const {
    return m.isRotated ? min(m.rect.sz_x - (*m.pr).sz_y, 
                             m.rect.sz_y - (*m.pr).sz_x) :
                         min(m.rect.sz_x - (*m.pr).sz_x,
                             m.rect.sz_y - (*m.pr).sz_y);
}
