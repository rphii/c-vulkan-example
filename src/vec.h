#ifndef VEC_SIMPLE_H

#include <sys/types.h>

/* functions for use {{{ */

void *vec_init(void);
void vec_free(void *vec);

#define vec_grow(vec, capacity)     _vec_grow(&vec, sizeof(*vec), capacity)
#define vec_push(vec, item)         (*(typeof(*vec) *)_vec_push(&vec, sizeof(*vec)) = item)

/*
#define vec_push(vec, item)         do { \
        typeof(*vec) *x = _vec_push(&vec, sizeof(*vec)); \
        println("x %p<-%u", x, item); \
        *x = item; \
    } while(0)
    */
#define vec_len(vec)                _vec_len(vec)
#define vec_cap(vec)                _vec_cap(vec)
#define vec_at(vec, index)          *(typeof(*vec) *)_vec_addr(vec, sizeof(*vec), index)

/*}}}*/

/* internal functions {{{ */

int _vec_grow(void *vec, size_t size, size_t capacity);

void *_vec_addr(void *vec, size_t size, size_t index);
void *_vec_push(void *vec, size_t size);

size_t _vec_len(void *vec);
size_t _vec_cap(void *vec);

/*}}}*/

#define VEC_SIMPLE_H
#endif

