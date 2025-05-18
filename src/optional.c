#include "optional.h"

void optional_u32_set(OptionalU32 *o, uint32_t val) {
    assert_arg(o);
    o->value = val;
    o->has_value = true;
}

void optional_u32_clear(OptionalU32 *o) {
    assert_arg(o);
    o->has_value = false;
}


