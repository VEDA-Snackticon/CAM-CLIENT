cmake_minimum_required(VERSION 3.16)

project(Snack_Guard_rev VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Multimedia MultimediaWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Multimedia MultimediaWidgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

# GStreamer 라이브러리 및 include 경로 설정
set(GSTREAMER_INCLUDE_DIRS
    "C:/gstreamer/1.0/msvc_x86_64/include/gstreamer-1.0"
    "C:/gstreamer/1.0/msvc_x86_64/include/glib-2.0"
    "C:/gstreamer/1.0/msvc_x86_64/lib/glib-2.0/include"
)
set(GSTREAMER_LIBRARIES
    "C:/gstreamer/1.0/msvc_x86_64/lib/gstreamer-1.0.lib"
    "C:/gstreamer/1.0/msvc_x86_64/lib/gobject-2.0.lib"
    "C:/gstreamer/1.0/msvc_x86_64/lib/glib-2.0.lib"
    "C:/gstreamer/1.0/msvc_x86_64/lib/gstvideo-1.0.lib"
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Snack_Guard_rev
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        mediaapp.h
        player.h
        mediaapp.cpp
        player.cpp
        image.qrc
        customwidget.h
        customwidget.cpp
        subwindow.h





    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Snack_Guard_rev APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Snack_Guard_rev SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Snack_Guard_rev
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(Snack_Guard_rev PRIVATE ${GSTREAMER_INCLUDE_DIRS})
target_link_libraries(Snack_Guard_rev PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Multimedia Qt${QT_VERSION_MAJOR}::MultimediaWidgets ${GSTREAMER_LIBRARIES})

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Snack_Guard_rev)
endif()
set_target_properties(Snack_Guard_rev PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Snack_Guard_rev
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Snack_Guard_rev)
endif()
