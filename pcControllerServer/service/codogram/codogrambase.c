#include "codogrambase.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static uint8_t codogramIdVsSize[CODOGRAM_ID_MAX] = {9, 2, 1};

uint8_t Codogram_getId(CodogramBase* codogram) {
    return codogram->data[0];
}

CodogramBase Codogram_construct(uint8_t* payload) {
    CodogramBase cdgBase;
    int8_t size = Codogram_getSizeByCodogramId(Codogram_getIdFromPayload(payload));
    memcpy_s(cdgBase.data, CODOGRAM_MAX_SIZE, payload, size);
    cdgBase.size = size;
    return cdgBase;
}

uint8_t Codogram_getIdFromPayload(uint8_t* payload) {
    return payload[0];
}

int8_t Codogram_getSizeByCodogramId(uint8_t id) {
    if(id >= CODOGRAM_ID_MAX)
        return -1;
    return codogramIdVsSize[id];
}
