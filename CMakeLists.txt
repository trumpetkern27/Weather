cmake_minimum_required(VERSION 3.16)

project(weather VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    find_package(Qt6 COMPONENTS Core Gui Widgets Network Positioning REQUIRED)

    qt_add_executable(weather
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        taskbarWindow.cpp
        taskbarWindow.h
        first_instance.h first_instance.cpp
        first_instance.ui
        helpers.h
        helpers.cpp
        dummy.h dummy.cpp
        fetch.h fetch.cpp
        cityprompt.h cityprompt.cpp cityprompt.ui
        settings_data.h settings_data.cpp settings_data.ui
        resources.qrc
        settings_main.h settings_main.cpp settings_main.ui
        change_location.h change_location.cpp change_location.ui

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET weather APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(weather SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(weather
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(weather PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network Qt6::Positioning shell32 user32 dwmapi)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.weather)
endif()
set_target_properties(weather PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS weather
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(weather)
endif()

