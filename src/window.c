#include "cwindow/window_base.h"
#include <cwindow/window.h>
#include <cwindow/context.h>
#include <cwindow/core.h>

typedef struct __cwindow {
    cwindow_context_t* context;
    cwindow_functions_t* functions;
    cwindow_allocator_t* allocator;
    // The inner window will be passed into the functions in cwindow_functions_t. This is the actual window handle.
    cwindow_t* inner_window;
} __cwindow_t;

cwindow_error_t cwindow_create(cwindow_context_t* context, cwindow_t** window, const cwindow_create_info_t* create_info) { 
    cwindow_allocator_t* allocator = cwindow_context_allocator(context);
    cwindow_functions_t* functions = cwindow_context_window_functions(context);

    __cwindow_t* __window = allocator->malloc(sizeof(__cwindow_t));

    if (!__window) {
        return CWINDOW_ERROR_ALLOC_FAILED;
    }

    __window->context = context;
    // Store references to the functions and allocator structs.
    __window->functions = functions;
    __window->allocator = allocator;

    cwindow_error_t res;
    if ((res = __window->functions->create(context, &__window->inner_window, create_info)) != CWINDOW_ERROR_SUCCESS) {
        allocator->free(__window);
        return res;
    }

    *window = (cwindow_t*)__window;

    return CWINDOW_ERROR_SUCCESS;
}

void cwindow_free(cwindow_t* window) {
    __cwindow_t* __window = (__cwindow_t*)window; 
    __window->functions->free(__window->inner_window);
    __window->allocator->free(__window);
}

cwindow_bool cwindow_should_close(cwindow_t* window) {
    __cwindow_t* __window = (__cwindow_t*)window; 
    return __window->functions->should_close(__window->inner_window);
}

void cwindow_poll_events(cwindow_t* window) {
    __cwindow_t* __window = (__cwindow_t*)window; 
    __window->functions->poll_events(__window->inner_window);
}
