#ifndef MOUSEMOVECODOGRAM_H
#define MOUSEMOVECODOGRAM_H

#include "codogrambase.h"

#include <stdio.h>

typedef struct {
    float x;
    float y;
} MousePoint;

static MousePoint MouseMoveCodogram_getMousePoint(CodogramBase cdgBase) {
    union {
        char charVal[4];
        float flVal;
    } convUnion;
     // todo: think about endianness
    for(int i = 0; i < 4; i++) {
       convUnion.charVal[i] = cdgBase.data[4 - i];
    }
    float x = convUnion.flVal;
    for(int i = 0; i < 4; i++) {
       convUnion.charVal[i] = cdgBase.data[8 - i];
    }
    float y = convUnion.flVal;

    MousePoint mousePoint = {
        .x = x,
        .y = y
    };
    // todo: probably it would be better by doing it by bit shifting
    return mousePoint;
}

#endif // MOUSEMOVECODOGRAM_H
