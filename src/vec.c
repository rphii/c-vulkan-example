#include "vec.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

static inline void *_vec_grow2(void *vec, size_t size, size_t capacity);

#define vec_base(vec)   ((vec) - offsetof(Vec, data))

typedef struct Vec {
    size_t length;
    size_t capacity;
    void *data;
} Vec;

void *vec_init(void) {
    Vec *v = malloc(sizeof(Vec));
    memset(v, 0, sizeof(*v));
#if 0
    println("allc v %p", v);
    println("     v.data %p", &v->data);
#endif
    return &v->data;
}

void vec_free(void *vec) {
    assert_arg(vec);
    Vec *v = vec_base(vec);
#if 0
    println("free v %p", v);
    println("     v.data %p", &v->data);
#endif
    free(v);
}

static inline void *_vec_grow2(void *vec, size_t size, size_t capacity) {
    assert_arg(vec);
    Vec *v = vec_base(vec);
    println("argp v.data %p / vec %p", &v->data, vec);
    if(capacity <= v->capacity) return vec;
    size_t require = 2;
    while(require < capacity) require *= 2;
    size_t bytes = sizeof(Vec) + size * require;
#if 1
    println("require %zu / cap %zu", require, v->capacity);
    println("bytes %zu (%zu + %zu * %zu)", bytes, sizeof(Vec), size, require);
#endif
    void *temp = realloc(v, bytes);
    if(!temp) return 0;
    v = temp;
    v->data = temp + offsetof(Vec, data);
    println("old  v.data %p..%p", &v->data, (void *)&v->data + size * v->capacity);
    println("zero v.data %p >> %zu bytes", (void *)&v->data + size * v->capacity, size * (require - v->capacity));
    memset((void *)&v->data + size * v->capacity, 0, size * (require - v->capacity));
    v->capacity = require;
#if 1
    //println("rllc v %p", v);
    //println("     v.data %p..%p", &v->data, &v->data + size * require);
#endif
    return &v->data;
}

int _vec_grow(void *vec, size_t size, size_t capacity) {
    void **p = vec;
    *p = _vec_grow2(*p, size, capacity);
    Vec *v = vec_base(*p);
    println("base %p data %p",v,&v->data);
    return !vec;
}

void *_vec_addr(void *vec, size_t size, size_t index) {
    assert_arg(vec);
#if !defined(NDEBUG)
    Vec *v = vec_base(vec);
    assert(index < v->length && "index is out of range!");
#endif
    return vec + size * index;
}

void *_vec_push(void *vec, size_t size) {
    void **p = vec;
    Vec *v = vec_base(*p);
    *p = _vec_grow2(*p, size, v->length + 1);
    v = vec_base(*p);
    size_t index = v->length++;
    //println("push        %p (%p + %zu * %zu)", (void *)&v->data + size * index, &v->data, size, index);
    //println("     v.data %p..%p", &v->data, &v->data+v->capacity*size);
    return (void *)&v->data + size * index;
}

//int _vec_push(void *vec, size_t size, void *item) {
//    _vec_addr(vec, size, vec_len(vec));
//}

size_t _vec_len(void *vec) {
    assert_arg(vec);
    Vec *v = vec_base(vec);
    return v->length;
}

size_t _vec_cap(void *vec) {
    assert_arg(vec);
    Vec *v = vec_base(vec);
    return v->capacity;
}



