#include <cwindow/core.h>
#include <cwindow/context.h>
#include <cwindow/window_base.h>
#include "wayland/window.h"

#include <stdlib.h>
#include <string.h>

typedef struct __cwindow_context {
    cwindow_functions_t functions;
    cwindow_allocator_t allocator;
} __cwindow_context_t;

cwindow_error_t cwindow_context_create(cwindow_context_t** context, cwindow_allocator_t* allocator) {
    cwindow_allocator_t alloc; 
    if (allocator) {
        alloc = *allocator;
    } else {
        alloc = cwindow_default_allocator();
    } 

    __cwindow_context_t* __context = alloc.malloc(sizeof(__cwindow_context_t));

    if (!__context) {
        return CWINDOW_ERROR_ALLOC_FAILED;
    }

    __context->allocator = alloc;


#if defined(__linux__)
    // x11 or wayland
    const char* session_type = getenv("XDG_SESSION_TYPE");

    if (!session_type) {
        return CWINDOW_ERROR_XDG_SESSION_TYPE_NOT_DEFINED;
    } 

    if (strcmp(session_type, "x11") == 0) {
        __context->allocator.free(__context);
        return CWINDOW_ERROR_NOT_IMPLEMENTED; 
    } else if (strcmp(session_type, "wayland") == 0) {
        __context->functions = cwindow_wl_functions();
    } else {
        __context->allocator.free(__context);
        return CWINDOW_ERROR_XDG_SESSION_TYPE_UNKOWN; 
    }
#endif

    *context = (cwindow_context_t*)__context;
        
    return CWINDOW_ERROR_SUCCESS;
}

void cwindow_context_free(cwindow_context_t* context) {
    ((__cwindow_context_t*)context)->allocator.free(context);
}

cwindow_allocator_t* cwindow_context_allocator(cwindow_context_t* context) {
    return &((__cwindow_context_t*)context)->allocator;
}

cwindow_functions_t* cwindow_context_window_functions(cwindow_context_t* context) {
    return &((__cwindow_context_t*)context)->functions;
}
