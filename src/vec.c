#include "vec.h"    /* keep as very first line */
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>
#include <stdio.h>

static inline void *vec_init(void);
static inline void *_vec_grow2(void *vec VEC_DEBUG_DEFS, size_t size, size_t capacity);

#define vec_assert_arg(arg)     assert(arg && "null pointer argument!");
#define vec_base(vec)   ((vec) - offsetof(Vec, data))

#define vec_error(msg, ...)     do { \
        printf("\n" VEC_DEBUG_FMT "vector error: " msg "\n" VEC_DEBUG_ARGS, ##__VA_ARGS__); \
        exit(1); \
    } while(0)

typedef struct Vec {
    size_t length;
    size_t capacity;
    void *data;
} Vec;

static inline void *vec_init(void) {
    Vec *v = malloc(sizeof(Vec));
    memset(v, 0, sizeof(*v));
    return &v->data;
}

static inline void *_vec_grow2(void *vec VEC_DEBUG_DEFS, size_t size, size_t capacity) {
    if(!vec) {
        vec = vec_init();
    }
    Vec *v = vec_base(vec);
    if(capacity <= v->capacity) return vec;
    if(capacity * 2 < capacity) {
        vec_error("invalid allocation size: %zu", capacity);
    }
    size_t require = 2;
    while(require < capacity) require *= 2;
    size_t bytes = sizeof(Vec) + size * require;
    if((bytes - sizeof(Vec)) / size != require) {
        vec_error("array member of %zu bytes can't allocate %zu elements", size, require);
    }
    void *temp = realloc(v, bytes);
    if(!temp) {
        vec_error("failed allocation of: %zu elements (%zu bytes)", require, bytes);
    }
    v = temp;
    memset((void *)&v->data + size * v->capacity, 0, size * (require - v->capacity));
    v->capacity = require;
    return &v->data;
}

void _vec_grow(void *vec VEC_DEBUG_DEFS, size_t size, size_t capacity) {
    vec_assert_arg(vec);
    void **p = vec;
    *p = _vec_grow2(*p VEC_DEBUG_ARGS, size, capacity);
}

void _vec_resize(void *vec VEC_DEBUG_DEFS, size_t size, size_t length) {
    vec_assert_arg(vec);
    void **p = vec;
    *p = _vec_grow2(*p VEC_DEBUG_ARGS, size, length);
    Vec *v = vec_base(*p);
    v->length = length;
}

void *_vec_addr(const void *vec VEC_DEBUG_DEFS, size_t size, size_t index) {
    vec_assert_arg(vec);
#if !defined(NDEBUG)
    Vec *v = (void *)vec_base(vec);
    if(!(index < v->length)) {
        vec_error("index %zu is out of bounds %zu", index, v->length);
    }
#endif
    return (void *)vec + size * index;
}

void *_vec_push(void *vec VEC_DEBUG_DEFS, size_t size) {
    void **p = vec; Vec *v = *p ? vec_base(*p) : 0;
    *p = _vec_grow2(*p VEC_DEBUG_ARGS, size, v ? v->length + 1 : 1);
    v = vec_base(*p);
    size_t index = v->length++;
    return (void *)&v->data + size * index;
}

void *_vec_pop(void *vec VEC_DEBUG_DEFS, size_t size) {
    vec_assert_arg(vec);
    Vec *v = vec_base(vec);
#if !defined(NDEBUG)
    if(!v->length) {
        vec_error("no elements left to pop");
    }
#endif
    size_t index = --v->length;
    return vec + size * index;
}

void _vec_free(void *vec) {
    vec_assert_arg(vec);
    void **p = vec;
    if(!*p) return;
    Vec *v = vec_base(*p);
    free(v);
    *p = 0;
}

size_t _vec_len(const void *vec) {
    if(!vec) return 0;
    Vec *v = (Vec *)vec_base(vec);
    return v->length;
}

size_t _vec_cap(const void *vec) {
    if(!vec) return 0;
    Vec *v = (Vec *)vec_base(vec);
    return v->capacity;
}

void _vec_clear(void *vec) {
    if(!vec) return;
    Vec *v = vec_base(vec);
    v->length = 0;
}
