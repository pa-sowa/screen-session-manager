cmake_minimum_required(VERSION 3.16)

project(ScreenSessionManager VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# On Ubuntu or Debian-based systems:
# sudo apt install libssh-dev
# On Fedora or Red Hat-based systems:
# sudo dnf install libssh-devel
# On Arch-based systems:
# sudo pacman -S libssh
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBSSH REQUIRED libssh)
include_directories(${LIBSSH_INCLUDE_DIRS})
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        src/main.cpp
        src/ICommandExecutor.h
        src/MainWidget.cpp src/MainWidget.h src/MainWidget.ui
        src/NewSessionDialog.h src/NewSessionDialog.cpp src/NewSessionDialog.ui
        src/ScreenManager.cpp src/ScreenManager.h
        src/ScreenSessionModel.cpp src/ScreenSessionModel.h
        src/ShellCommandExecutor.cpp src/ShellCommandExecutor.h
        src/SshCommandExecutor.cpp src/SshCommandExecutor.h
        src/SshHostConfig.h src/SshHostConfig.cpp
        src/SshSession.cpp src/SshSession.h
        src/SshHostConfigDialog.cpp src/SshHostConfigDialog.h src/SshHostConfigDialog.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ScreenSessionManager
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ScreenSessionManager APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ScreenSessionManager SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ScreenSessionManager
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(ScreenSessionManager PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    ${LIBSSH_LIBRARIES}
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ScreenSessionManager)
endif()
set_target_properties(ScreenSessionManager PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ScreenSessionManager
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ScreenSessionManager)
endif()
