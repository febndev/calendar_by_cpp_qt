cmake_minimum_required(VERSION 3.16)

project(calendarServer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Sql Concurrent)
find_package(Qt6 COMPONENTS Core Gui Network REQUIRED)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui

)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(calendarServer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}

        server.h server.cpp
        calendarutils.h calendarutils.cpp
        login.h login.cpp
        signup.h signup.cpp
        event.h event.cpp
        logo.h logo.cpp
        dbmanager.h dbmanager.cpp

    )
# 리소스 파일이 있으면 추가 (나중에 추가할 때)
 if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc")
     list(APPEND PROJECT_SOURCES resources.qrc)
 endif()

# Define target properties for Android with Qt 6 as:
#    set_property(TARGET calendarClient APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(calendarClient SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(calendarClient
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(calendarServer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(calendarServer PRIVATE Qt6::Core Qt6::Widgets)
target_link_libraries(calendarServer PRIVATE Qt6::Sql Qt6::Concurrent)
target_link_libraries(calendarServer PRIVATE Qt6::Core Qt6::Gui Qt6::Network)

# Network 라이브러리 링크 (TCP/IP 통신용)
if(TARGET Qt6::Network)
    target_link_libraries(calendarServer PRIVATE Qt6::Network)
endif()

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.calendarClient)
endif()
set_target_properties(calendarServer PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS calendarServer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(calendarServer)
endif()
