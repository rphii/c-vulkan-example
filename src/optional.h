#ifndef OPTIONAL_H

#include <stdint.h>
#include <stdbool.h>
#include "util.h"

typedef struct OptionalU32 {
    uint32_t value;
    bool has_value;
} OptionalU32;

void optional_u32_set(OptionalU32 *o, uint32_t val);
void optional_u32_clear(OptionalU32 *o);

#define OPTIONAL_H
#endif

