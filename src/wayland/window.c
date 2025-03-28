#include "window.h"
#include "cwindow/core.h"
#include <cwindow/context.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/mman.h>

#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include <wayland-egl.h>
#include <xkbcommon/xkbcommon.h>

typedef struct __cwindow_wl {
    cwindow_allocator_t* allocator;
    struct wl_display* display;
    struct wl_registry* registry;
    struct wl_compositor* compositor;
    struct wl_surface* surface;
    struct xdg_wm_base* xdg_wm_base;
    struct xdg_surface* xdg_surface;
    struct xdg_toplevel* xdg_toplevel;
    struct wl_seat* seat;
    struct wl_keyboard* keyboard;
    struct wl_pointer* pointer;
    struct wl_keyboard_listener keyboard_listener;
    struct wl_pointer_listener pointer_listener;
    struct xkb_context* xkb_context;
    struct xkb_keymap* xkb_keymap;
    struct xkb_state* xkb_state;
    int32_t width, height;
    uint32_t max_width, max_height;
    int32_t mouse_x, mouse_y;
    cwindow_state_t state;
    cwindow_bool should_close;
} __cwindow_wl_t;

// To check if the application deadlocked xdg will send a ping event and we respond with a pong event.
static void xdg_wm_base_ping(void* data, struct xdg_wm_base* xdg_wm_base, uint32_t serial) {
    CWINDOW_UNUSED_PARAM(data);
    xdg_wm_base_pong(xdg_wm_base, serial);
}

static void xdg_surface_configure(void* data, struct xdg_surface* xdg_surface, uint32_t serial) {
    CWINDOW_UNUSED_PARAM(data);

    xdg_surface_ack_configure(xdg_surface, serial);
}

static void xdg_toplevel_close(void* data, struct xdg_toplevel* toplevel) {
    CWINDOW_UNUSED_PARAM(toplevel);

    __cwindow_wl_t* __window = (__cwindow_wl_t*)data;
    __window->should_close = CWINDOW_TRUE;
}

static void xdg_toplevel_configure(void* data, struct xdg_toplevel* toplevel, int32_t width, int32_t height, struct wl_array* states) {
    CWINDOW_UNUSED_PARAM(toplevel);
    CWINDOW_UNUSED_PARAM(states);

    if (width == 0 || height == 0) return;

    __cwindow_wl_t* __window = (__cwindow_wl_t*)data;
    __window->width = width;
    __window->height = height;
}

static void wl_keyboard_keymap(void* data, struct wl_keyboard* keyboard, uint32_t format, int32_t fd, uint32_t size) {
    assert(format == WL_KEYBOARD_KEYMAP_FORMAT_NO_KEYMAP);

    __cwindow_wl_t* __window = (__cwindow_wl_t*)data;

    // Map the keymap from a file to system memory.
    char* map_shm = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    assert(map_shm != MAP_FAILED && "Failed to map the xkb keymap provided to system memory");

    struct xkb_keymap* xkb_keymap = xkb_keymap_new_from_string(__window->xkb_context, map_shm, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);

    // Unmap memory
    munmap(map_shm, size);

    close(fd);

    if (__window->xkb_state != NULL) {
        xkb_keymap_unref(__window->xkb_keymap);
        xkb_state_unref(__window->xkb_state);
        __window->xkb_keymap = xkb_keymap;
        __window->xkb_state = xkb_state_new(__window->xkb_keymap);
    }

    __window->xkb_keymap = xkb_keymap;
    __window->xkb_state = xkb_state_new(__window->xkb_keymap);
 }

static void wl_keyboard_key(void* data, struct wl_keyboard* keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state) {

}

// A mod is depressed when its being pressed down, a latched mod is something like shift when sticky keys are on. It is active until a non-mod key is pressed.
// A locked mod is like caps lock / num lock. Since other window api's do not support this CWindow will not either. All mods will either be down or up.
static void wl_keyboard_modifiers(void* data, struct wl_keyboard* keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group) {
    __cwindow_wl_t* window = (__cwindow_wl_t*)data;
    xkb_state_update_mask(window->xkb_state, mods_depressed, mods_latched, mods_locked, 0, 0, group);
}

static void wl_keyboard_enter(void* data, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface* surface, struct wl_array* keys) {

}

static void wl_keyboard_leave(void* data, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface* surface) {

}

static void wl_keyboard_repeat_info(void* data, struct wl_keyboard* keyboard, int32_t rate, int32_t delay) {

}

static void wl_seat_capabilities(void* data, struct wl_seat* seat, uint32_t capabilities) {
    __cwindow_wl_t* __window = (__cwindow_wl_t*)data;

    cwindow_bool have_pointer = capabilities & WL_SEAT_CAPABILITY_POINTER;
    cwindow_bool have_keyboard = capabilities & WL_SEAT_CAPABILITY_KEYBOARD;

    // If the wl_seat has the capabilities for a pointer than create a pointer and input listeners (mouse was connected).
    if (have_pointer && __window->pointer == NULL) {
        __window->pointer = wl_seat_get_pointer(__window->seat);
        // TODO: Uncomment line when pointer listener functions are defined.
        // wl_pointer_add_listener(__window->pointer, &__window->pointer_listener, __window);
    } 
    // If the wl_seat does not have the capabilities for a pointer and a pointer exists (mouse was disconnected) release the pointer.
    else if (!have_pointer && __window->pointer != NULL) {
        wl_pointer_release(__window->pointer);
        __window->pointer = NULL;
    }

    if (have_keyboard && __window->keyboard == NULL) {
        __window->keyboard = wl_seat_get_keyboard(__window->seat);
        wl_keyboard_add_listener(__window->keyboard, &__window->keyboard_listener, __window);
    } 
    else if (!have_keyboard && __window->keyboard != NULL) {
        wl_keyboard_release(__window->keyboard);
        __window->keyboard = NULL;
    }
}

static void wl_seat_name(void* data, struct wl_seat* seat, const char* name) {
    CWINDOW_UNUSED_PARAM(data);
    CWINDOW_UNUSED_PARAM(seat);

    printf("struct wl_seat name: %s", name);
}

static void registry_handle_global(void* data, struct wl_registry* registry, uint32_t name, const char* interface, uint32_t version) {
    CWINDOW_UNUSED_PARAM(version);

    __cwindow_wl_t* __window = (__cwindow_wl_t*)data;

    if (strcmp(interface, wl_compositor_interface.name) == 0) {
        // Specify version 4 which is the latest version used in the wayland-book.
        __window->compositor = wl_registry_bind(registry, name, &wl_compositor_interface, 4);
    } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        const struct xdg_wm_base_listener xdg_wm_base_listener = {
            .ping = xdg_wm_base_ping,
        };

        __window->xdg_wm_base = wl_registry_bind(registry, name, &xdg_wm_base_interface, 1) ;
        xdg_wm_base_add_listener(__window->xdg_wm_base, &xdg_wm_base_listener, NULL);
    } else if (strcmp(interface, wl_seat_interface.name) == 0) {
        const struct wl_seat_listener wl_seat_listener = {
            .capabilities = wl_seat_capabilities,
            .name = wl_seat_name,
        };

        __window->seat = wl_registry_bind(registry, name, &wl_seat_interface, 7);
        wl_seat_add_listener(__window->seat, &wl_seat_listener, __window);
    }

}

static void registry_handle_global_remove(void* data, struct wl_registry* registry, uint32_t name) {
    CWINDOW_UNUSED_PARAM(data);
    CWINDOW_UNUSED_PARAM(registry);
    CWINDOW_UNUSED_PARAM(name);
}

cwindow_error_t cwindow_wl_create(cwindow_context_t* context, cwindow_t** window, const cwindow_create_info_t* create_info) { 
    cwindow_allocator_t* allocator = cwindow_context_allocator(context);

    __cwindow_wl_t* __window = allocator->malloc(sizeof(__cwindow_wl_t));

    if (!__window) {
        return CWINDOW_ERROR_ALLOC_FAILED;
    }

    __window->allocator = allocator;

    __window->display = wl_display_connect(NULL);

    if (!__window->display) {
        return CWINDOW_ERROR_WAYLAND_FAILED_TO_CONNECT_TO_DISPLAY;
    }

    const struct wl_registry_listener registry_listener = {
        .global = registry_handle_global,
        .global_remove = registry_handle_global_remove
    };

    __window->keyboard_listener = (struct wl_keyboard_listener){
        .keymap = wl_keyboard_keymap,
        .key = wl_keyboard_key,
        .modifiers = wl_keyboard_modifiers,
        .enter = wl_keyboard_enter,
        .leave = wl_keyboard_leave,
        .repeat_info = wl_keyboard_repeat_info,
    };

    // TODO: Create pointer listener
    __window->pointer_listener = (struct wl_pointer_listener){ 0 };

    __window->registry = wl_display_get_registry(__window->display);
    wl_registry_add_listener(__window->registry, &registry_listener, __window);

    // Block the thread until all commands sent are done being processed. This is so we can make sure the we got the compositor, xdg_wm_base, ...
    wl_display_roundtrip(__window->display);

    __window->surface = wl_compositor_create_surface(__window->compositor);
    __window->xdg_surface = xdg_wm_base_get_xdg_surface(__window->xdg_wm_base, __window->surface);

    const struct xdg_surface_listener xdg_surface_listener = {
        .configure = xdg_surface_configure
    };

    xdg_surface_add_listener(__window->xdg_surface, &xdg_surface_listener, __window);

    __window->xdg_toplevel = xdg_surface_get_toplevel(__window->xdg_surface);
    xdg_toplevel_set_title(__window->xdg_toplevel, create_info->title);

    const struct xdg_toplevel_listener xdg_toplevel_listener = {
        .close = xdg_toplevel_close,
        .configure = xdg_toplevel_configure,

    };

    xdg_toplevel_add_listener(__window->xdg_toplevel, &xdg_toplevel_listener, __window);

    __window->xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);

    *window = (cwindow_t*)__window;

    return CWINDOW_ERROR_SUCCESS;
}

// TODO: Clean up all objects
void cwindow_wl_free(cwindow_t* window) {
    __cwindow_wl_t* __window = (__cwindow_wl_t*)window; 
    wl_surface_destroy(__window->surface);
    wl_compositor_destroy(__window->compositor);
    wl_registry_destroy(__window->registry);
    wl_display_disconnect(__window->display);
    __window->allocator->free(__window);
}

cwindow_bool cwindow_wl_should_close(cwindow_t* window) {
    return ((__cwindow_wl_t*)window)->should_close;
}


void cwindow_wl_poll_events(cwindow_t* window) {
    wl_display_dispatch(((__cwindow_wl_t*)window)->display);
}
