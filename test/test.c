#include "cwindow/core.h"
#include <cwindow/window.h>
#include <cwindow/context.h>
#include <stdio.h>

int main() {
    cwindow_context_t* context;
    cwindow_error_t result = cwindow_context_create(&context, NULL);

    if (result != CWINDOW_ERROR_SUCCESS) {
        fprintf(stderr, "Failed to create context: %u\n", (uint32_t)result);
        return EXIT_FAILURE;
    }

    cwindow_create_info_t create_info = {
        .width = 1280,
        .height = 720,
        .x = CW_POSITION_CENTER,
        .y = CW_POSITION_CENTER,
        .title = "CWindow Test"
    };

    cwindow_t* window; 
    result = cwindow_create(context, &window, &create_info);

    if (result != CWINDOW_ERROR_SUCCESS) {
        fprintf(stderr, "Failed to create window: %u\n", (uint32_t)result);
        return EXIT_FAILURE;
    }

    while (!cwindow_should_close(window)) {
        cwindow_poll_events(window);
    }

    cwindow_free(window);
    cwindow_context_free(context);

    return 0;
}
