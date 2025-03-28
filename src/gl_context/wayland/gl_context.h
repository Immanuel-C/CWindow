#if !defined(GL_CONTEXT_WAYLAND_GL_CONTEXT)
#define GL_CONTEXT_WAYLAND_GL_CONTEXT 

#include <cwindow/core.h>

CWINDOW_HANDLE(cwindow_gl_context)

cwindow_error_t cw_gl_context_create();
cwindow_error_t cw_gl_context_free();

#endif
