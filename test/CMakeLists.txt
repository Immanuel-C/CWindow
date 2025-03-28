add_compile_definitions(_CRT_SECURE_NO_DEPRECATE)


add_executable(CWindowTest 
    test.c 
)

target_include_directories(CWindowTest
    PUBLIC ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(CWindowTest
    PUBLIC CWindow 
)

if (MSVC)
    target_compile_options(CWindowTest PRIVATE /W4)
else()
    target_compile_options(CWindowTest PRIVATE -Wall PRIVATE -Wpedantic PRIVATE -Wextra )
endif()
