#ifndef LOG_H

#include <time.h>
#include <stdbool.h>
#include "util.h"

typedef struct Log {
    int level;
    struct timespec t0;
    struct timespec tE;
    bool enable_output;
} Log;

#define LOG_PRINT(ch, log, msg, ...)    \
    println("%*s" ch " " msg F(" %.4fs", IT FG_BK_B), (log)->level, "", ##__VA_ARGS__, log_t_sec(log)); \

#define log_down(log, msg, ...)   do { \
        if(!(log)->enable_output) break; \
        _log_down(log); \
        LOG_PRINT(F(">", FG_BL_B), log, msg, ##__VA_ARGS__); \
    } while(0)

void log_output(Log *log, bool enable);
void log_start(Log *log);
void log_up(Log *log);
void _log_ok(Log *log);
void _log_down(Log *log);
void _log_info(Log *log);

double log_t_sec(Log *log);

#define log_info(log, msg, ...)    do { \
        if(!(log)->enable_output) break; \
        _log_info(log); \
        LOG_PRINT(F("-", FG_BL_B), log, msg, ##__VA_ARGS__); \
    } while(0)

#define log_ok(log, msg, ...)       do { \
        if(!(log)->enable_output) break; \
        _log_ok(log); \
        LOG_PRINT(F("*", FG_GN_B BOLD), log, msg, ##__VA_ARGS__); \
    } while(0)


#define LOG_H
#endif

