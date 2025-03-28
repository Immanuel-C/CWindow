#if !defined(TIMER_H_UTIL)
#define TIMER_H_UTIL 

#include <cwindow/core.h>

/**
 * @struct An opaque handle that holds time data to calculate the time that has passed in seconds. 
 */
CWINDOW_HANDLE(cw_timer);

/**
 * @brief Create a cw_timer and start the timer.
 * @param[out] timer A reference to an empty cw_timer.
 * @param[in] allocator The allocator that will allocate the timer.
 * @retval cw_error CW_ERROR_ALLOC_FAILED will be returned if allocating the timer failed. CW_ERROR_SUCCESS if the function succeeded.
 */
CWINDOW_API cw_error cw_timer_start(cw_timer** timer, cw_allocator allocator);
/**
 * @brief Get the difference of the current time minus the starting time.
 * @param[in] timer A reference to a valid cw_timer.
 * @retval double The difference if time between the time start and the current time.
 */
CWINDOW_API double cw_timer_diff(cw_timer* timer);
/**
 * @brief Get the difference of the current time minus the starting time and free the cw_timer.
 * @param[in] timer A reference to a valid cw_timer.
 * @retval double The difference if time between the time start and the current time.
 */
CWINDOW_API double cw_timer_end(cw_timer* timer);

#endif
