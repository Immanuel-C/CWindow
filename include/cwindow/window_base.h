#if !defined(WINDOW_BASE_H_CWINDOW_INCLUDE)
#define WINDOW_BASE_H_CWINDOW_INCLUDE

#include "core.h"

/**
 * @struct An opaque handle that will handle native window creation, destruction, etc... on a platform agnostic api.
 */
CWINDOW_HANDLE(cwindow_t)
CWINDOW_HANDLE(cwindow_context_t)

/**
 * @brief Set the x and y fields of cw_window_create_info to CW_POSITION_CENTER to create the window at the center of the primary monitor.
 */
#define CW_POSITION_CENTER INT32_MAX

/**
 *  @brief Let CWindow decide what a variable should be instead of the user. 
 */
#define CW_DECIDE INT32_MAX

/**
 * @struct A struct that contains all the information to create a window. 
 */
typedef struct cwindow_create_info {
    uint32_t width, height;
    int32_t x, y;
    const char* title;
} cwindow_create_info_t;

/**
 * @struct A struct that contains the current window "state".
 */
typedef enum cwindow_state {
    CW_WINDOW_STATE_NORMAL,
    CW_WINDOW_STATE_MAXIMIZED,
    CW_WINDOW_STATE_MINIMIZED,
    CW_WINDOW_STATE_FULLSCREEN,
} cwindow_state_t;


typedef cwindow_error_t (*cwindow_create_pfn_t)(cwindow_context_t* context, cwindow_t** window, const cwindow_create_info_t* create_info);
typedef void (*cwindow_free_pfn_t)(cwindow_t* window);
typedef cwindow_bool (*cwindow_should_close_pfn_t)(cwindow_t* window);
typedef void (*cwindow_poll_events_pfn_t)(cwindow_t* window);

typedef struct cwindow_functions {
    cwindow_create_pfn_t create; 
    cwindow_free_pfn_t free; 
    cwindow_should_close_pfn_t should_close;
    cwindow_poll_events_pfn_t poll_events;
} cwindow_functions_t;

#endif
