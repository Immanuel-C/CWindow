find_package(WaylandScanner REQUIRED)

file(GLOB PROTOCOL_XMLS "${CMAKE_BINARY_DIR}/deps/wayland/*.xml")

set(PROTOCOL_HEADER_FILES "${CMAKE_BINARY_DIR}/deps/wayland/wayland-client-protocol.h")
set(PROTOCOL_HEADER_FILES_DIR "${CMAKE_BINARY_DIR}/deps/wayland/")
set(PROTOCOL_SOURCE_FILES  "${CMAKE_BINARY_DIR}/deps/wayland/wayland-client-protocol.c" )

add_compile_definitions(
    _CRT_SECURE_NO_DEPRECATE 
    CWINDOW_WAYLAND
)

add_library(CWindow STATIC
    window.c    
    wayland/window.c    
    context.c
    gl_context/wayland/gl_context.c
    ${PROTOCOL_SOURCE_FILES}
)

target_include_directories(CWindow 
    PUBLIC ${CMAKE_SOURCE_DIR}/include
    PUBLIC ${CMAKE_SOURCE_DIR}/src
    PUBLIC ${PROTOCOL_HEADER_FILES_DIR}
    PUBLIC ${XKBCOMMON_INCLUDE_DIRS}
)

target_link_libraries(CWindow
    PUBLIC wayland-client
    PUBLIC ${XKBCOMMON_LIBRARIES}
)

add_custom_command(
    TARGET CWindow
    PRE_BUILD 
    COMMAND ${WaylandScanner_EXECUTABLE} client-header ${PROTOCOL_XMLS} ${PROTOCOL_HEADER_FILES}
    BYPRODUCTS ${PROTOCOL_HEADER_FILES} 
    COMMENT "CWindow: Generating wayland protocol headers. Wayland protocols provided: ${PROTOCOL_XMLS}"
    USES_TERMINAL
    VERBATIM
)

add_custom_command(
    TARGET CWindow
    PRE_BUILD
    COMMAND ${WaylandScanner_EXECUTABLE} private-code ${PROTOCOL_XMLS} ${PROTOCOL_SOURCE_FILES}
    BYPRODUCTS ${PROTOCOL_HEADER_FILES} 
    COMMENT "CWindow: Generating wayland protocol source. Wayland protocols provided: ${PROTOCOL_XMLS}"
    USES_TERMINAL
    VERBATIM
)

if (MSVC)
    target_compile_options(CWindow PRIVATE /W4)
else()
    target_compile_options(CWindow PRIVATE -Wall PRIVATE -Wpedantic PRIVATE -Wextra )
endif()
