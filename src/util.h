#ifndef UTIL_H

#include <stdio.h>
#include <assert.h>

#define try(fn)             do { if(fn) { println("%s:%d:%s failed", __FILE__, __LINE__, __func__); goto error; }} while(0)
#define assert_arg(arg)     assert(arg && "null pointer argument!");
#define println(msg, ...)   printf(msg "\n", ##__VA_ARGS__);

#define UTIL_H
#endif

