# Add executable
find_package(Curses)

if(CURSES_FOUND)
    add_executable(p-xing main.c pbm.c pxing.c render.c)

    # Include directories
    target_include_directories(p-xing PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${CURSES_INCLUDE_DIR}
    )

    target_link_libraries(p-xing ${CURSES_LIBRARIES})
else()
    message(WARNING "Curses not found; skipping p-xing TUI target. Logic tests can still be built.")
endif()
