#set(CMAKE_PREFIX_PATH /usr/local)

add_library(TerminalLib STATIC
    celliterator.cpp celliterator.h
    glyphcache.cpp glyphcache.h
    keys.cpp keys.h
    scrollback.cpp scrollback.h
    surfaceintegration.h
    terminal_global.h
    terminal.qrc
    terminalsurface.cpp terminalsurface.h
    terminalview.cpp terminalview.h
)

# find_package(PkgConfig REQUIRED)
# pkg_check_modules(VTERM REQUIRED vterm)

# if (VTERM_FOUND)
#     target_include_directories(TerminalLib PRIVATE ${VTERM_INCLUDE_DIRS})
#     target_link_libraries(TerminalLib PRIVATE ${VTERM_LIBRARIES})
# else()
#     message(FATAL_ERROR "libvterm not found via pkg-config")
# endif()


add_subdirectory(libvterm)

#target_include_directories(TerminalLib PRIVATE ${CMAKE_CURRENT_LIST_DIR}/libvterm/include)

target_link_libraries(TerminalLib
  PRIVATE
    Qt::Core
    Qt::Widgets
    libvterm_priv
)
