#include "core.h"
#include "cwindow/window_base.h"

cwindow_error_t cwindow_context_create(cwindow_context_t** context, cwindow_allocator_t* allocator);
void cwindow_context_free(cwindow_context_t* context);

cwindow_allocator_t* cwindow_context_allocator(cwindow_context_t* context);
cwindow_functions_t* cwindow_context_window_functions(cwindow_context_t* context);
