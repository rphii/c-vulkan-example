#ifndef V_H

#include <rphii/vec.h>
#include <vulkan/vulkan.h>

#define V_INCL(x)    \
    VEC_INCLUDE(V##x, v##x, x, BY_VAL, BASE) \
    VEC_INCLUDE(V##x, v##x, x, BY_VAL, ERR)

#define V_IMPL(x)    \
    VEC_IMPLEMENT(V##x, v##x, x, BY_VAL, BASE, 0) \
    VEC_IMPLEMENT(V##x, v##x, x, BY_VAL, ERR)

#define V_H
#endif

 
