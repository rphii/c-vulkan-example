#ifndef VEC_SIMPLE_H

#include <stddef.h>

/* debug optimization {{{ */

#if defined(NDEBUG)
#define VEC_DEBUG_INFO
#define VEC_DEBUG_ARGS
#define VEC_DEBUG_DEFS
#define VEC_DEBUG_FMT
#else
#define VEC_DEBUG_INFO  , __FILE__, __LINE__, __func__
#define VEC_DEBUG_ARGS  , file, line, func
#define VEC_DEBUG_DEFS  , const char *file, const int line, const char *func
#define VEC_DEBUG_FMT   "%s:%u:%s() "
#endif

/*}}}*/

/* functions for use {{{ */

#define vec_grow(vec, capacity)     _vec_grow(&vec VEC_DEBUG_INFO, sizeof(*vec), capacity)
#define vec_resize(vec, length)     _vec_resize(&vec VEC_DEBUG_INFO, sizeof(*vec), length)
#define vec_push(vec, item)         (*(typeof(vec))_vec_push(&vec VEC_DEBUG_INFO, sizeof(*vec)) = item)
#define vec_pop(vec, item)          *(typeof(vec))_vec_pop(vec VEC_DEBUG_INFO, sizeof(*vec))
#define vec_at(vec, index)          *(typeof(vec))_vec_addr(vec VEC_DEBUG_INFO, sizeof(*vec), index)
#define vec_len(vec)                _vec_len(vec)
#define vec_cap(vec)                _vec_cap(vec)
#define vec_clear(vec)              _vec_clear(vec)
#define vec_free(vec)               _vec_free(&vec)

/*}}}*/

/* internal functions {{{ */

void _vec_grow(void *vec VEC_DEBUG_DEFS, size_t size, size_t capacity);
void _vec_resize(void *vec VEC_DEBUG_DEFS, size_t size, size_t length);
void *_vec_push(void *vec VEC_DEBUG_DEFS, size_t size);
void *_vec_pop(void *vec VEC_DEBUG_DEFS, size_t size);
void *_vec_addr(const void *vec VEC_DEBUG_DEFS, size_t size, size_t index);
size_t _vec_len(const void *vec);
size_t _vec_cap(const void *vec);
void _vec_clear(void *vec);
void _vec_free(void *vec);

/*}}}*/

#define VEC_SIMPLE_H
#endif

