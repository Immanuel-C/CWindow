#if !defined(CORE_H_CWINDOW_INCLUDE)
#define CORE_H_CWINDOW_INCLUDE 

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#if defined(CWINDOW_DLL)
#if defined(CWINDOW_EXPORT_DLL)
#if defined(_MSC_VER) || defined(__clang__)
#define CWINDOW_API __declspec(dllexport)
#else
#define CWINDOW_API __attribute__((dllexport))
#endif
#else 
#if defined(_MSC_VER) || defined(__clang__)
#define CWINDOW_API __declspec(dllimport)
#else 
#define CWINDOW_API __attribute__((dllimport))
#endif
#endif
#else 
#define CWINDOW_API 
#endif

#define CWINDOW_HANDLE(x) typedef struct x x;
#define CWINDOW_UNUSED_PARAM(x) (void)x

typedef void* (*cwindow_malloc_pfn)(size_t);
typedef void* (*cwindow_calloc_pfn)(size_t, size_t);
typedef void* (*cwindow_realloc_pfn)(void*, size_t);
typedef void  (*cwindow_free_pfn)(void*);

typedef struct cwindow_allocator {
    cwindow_malloc_pfn malloc;
    cwindow_calloc_pfn calloc;
    cwindow_realloc_pfn realloc;
    cwindow_free_pfn free;
} cwindow_allocator_t;

static inline cwindow_allocator_t cwindow_default_allocator() {
    return (cwindow_allocator_t){
        .malloc = malloc,
        .calloc = calloc,
        .realloc = realloc,
        .free = free,
    };
}

typedef enum cwindow_error {
    CWINDOW_ERROR_SUCCESS,
    CWINDOW_ERROR_NOT_IMPLEMENTED,
    CWINDOW_ERROR_ALLOC_FAILED,
    CWINDOW_ERROR_WAYLAND_FAILED_TO_CONNECT_TO_DISPLAY,
    CWINDOW_ERROR_XDG_SESSION_TYPE_NOT_DEFINED,
    CWINDOW_ERROR_XDG_SESSION_TYPE_UNKOWN,
} cwindow_error_t;

typedef uint8_t cwindow_bool;
enum cwindow_bool_val {
    CWINDOW_FALSE = UINT8_C(0),
    CWINDOW_TRUE = UINT8_C(1),
};

#endif
