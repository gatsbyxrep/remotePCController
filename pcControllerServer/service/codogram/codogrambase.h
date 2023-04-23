#ifndef CODOGRAMBASE_H
#define CODOGRAMBASE_H

#include <stdint.h>
#include "codogramids.h"
typedef enum {
    CODOGRAM_MAX_SIZE = 9
} CodogramInfo;

static uint8_t codogramIdVsSize[CODOGRAM_ID_MAX];

typedef struct {
  uint8_t data[CODOGRAM_MAX_SIZE];
  int size;
} CodogramBase;


CodogramBase Codogram_construct(uint8_t* payload);
uint8_t Codogram_getId(CodogramBase* codogram);
static uint8_t Codogram_getIdFromPayload(uint8_t* payload);
static int8_t Codogram_getSizeByCodogramId(uint8_t id);


#endif // CODOGRAMBASE_H
