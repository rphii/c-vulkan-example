#include "log.h"

void log_output(Log *log, bool enable) {
    log->enable_output = enable;
}

void log_start(Log *log) {
    clock_gettime(CLOCK_REALTIME, &log->t0);
    log->level = -2;
    log_output(log, true);
}

void log_t_update(Log *log) {
    assert_arg(log);
    clock_gettime(CLOCK_REALTIME, &log->tE);
}

void log_up(Log *log) {
    assert_arg(log);
    log_t_update(log);
    log->level -= 2;
}

void _log_ok(Log *log) {
    assert_arg(log);
    log_t_update(log);
}

void _log_down(Log *log) {
    assert_arg(log);
    log_t_update(log);
    log->level += 2;
}

void _log_info(Log *log) {
    assert_arg(log);
    log_t_update(log);
}

struct timespec diff_timespec(const struct timespec *time1,
    const struct timespec *time0) {
  assert(time1);
  assert(time0);
  struct timespec diff = {.tv_sec = time1->tv_sec - time0->tv_sec, //
      .tv_nsec = time1->tv_nsec - time0->tv_nsec};
  if (diff.tv_nsec < 0) {
    diff.tv_nsec += 1000000000; // nsec/sec
    diff.tv_sec--;
  }
  return diff;
}

double log_t_sec(Log *log) {
    assert_arg(log);
    struct timespec delta = diff_timespec(&log->tE, &log->t0);
    return (double)delta.tv_sec + (double)delta.tv_nsec / 1e9;
}


