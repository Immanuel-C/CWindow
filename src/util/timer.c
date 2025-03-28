#include "timer.h"
#include "cwindow/core.h"
#include <bits/time.h>
#include <time.h>

typedef struct __cw_timer {
    const struct timespec start;
    struct timespec curr;
    cw_allocator allocator;
} __cw_timer;

cw_error cw_timer_start(cw_timer** timer, cw_allocator allocator) {
    __cw_timer* __timer = allocator.malloc(sizeof(__cw_timer));

    if (!__timer) {
        return CW_ERROR_ALLOC_FAILED;
    }

    __timer->allocator = allocator;
    // We never want to change the start time after this point. Thats why its const.
    clock_gettime(CLOCK_BOOTTIME, (struct timespec*)&__timer->start);

    *timer = (cw_timer*)__timer;

    return CW_ERROR_SUCCESS;
}

double cw_timer_diff(cw_timer* timer) {
    __cw_timer* __timer = (__cw_timer*)timer;
    clock_gettime(CLOCK_BOOTTIME, &__timer->curr);
    
    return (__timer->curr.tv_sec - __timer->start.tv_sec) + (__timer->curr.tv_nsec - __timer->start.tv_nsec) / 1e9;
}

double cw_timer_end(cw_timer* timer) {
    __cw_timer* __timer = (__cw_timer*)timer;
    clock_gettime(CLOCK_BOOTTIME, &__timer->curr);

    double diff = (__timer->curr.tv_sec - __timer->start.tv_sec) + (__timer->curr.tv_nsec - __timer->start.tv_nsec) / 1e9;

    __timer->allocator.free(__timer);
    
    return diff;
}
