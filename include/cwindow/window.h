#if !defined(WINDOW_H_CWINDOW_INCLUDE)
#define WINDOW_H_CWINDOW_INCLUDE

#include "core.h"
#include "window_base.h"

/**
 * @brief Create a cw_window.
 * @param[out] Outputs a valid cw_window if the function succeeds.
 * @param[in] The info to create the cw_window.
 * @retval cw_error Returns CW_ERROR_ALLOC_FAILED if any internal allocations fail. Returns CW_ERROR_WAYLAND_FAILED_TO_CONNECT_TO_DISPLAY if the application could not connect to a wayland display. Returns CW_SUCCESS if the function succeeded.
 */
cwindow_error_t cwindow_create(cwindow_context_t* context, cwindow_t** window, const cwindow_create_info_t* create_info);
/**
 * @brief Free a cw_window and all its dependencies.
 * @param[in] The valid cw_window that should be freed.
 */
void cwindow_free(cwindow_t* window);
/**
 * @brief Check if the window should close.
 * @param[in] A valid cw_window.
 * @retval cw_bool CW_TRUE if the window should close and CW_FALSE if it should not close.
 */
cwindow_bool cwindow_should_close(cwindow_t* window);
/**
 * @brief Poll the native window system for events.
 * @param[in] A valid cw_window
 */
void cwindow_poll_events(cwindow_t* window);

#endif
