cmake_minimum_required(VERSION 3.16)

project(calendar VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Sql)
find_package(Qt6 COMPONENTS Core Gui Network REQUIRED)


# Network 컴포넌트 추가 (TCP/IP 통신용)
find_package(Qt6 COMPONENTS Network)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        loginwindow.h loginwindow.cpp loginwindow.ui
        signupwindow.h signupwindow.cpp signupwindow.ui
        eventdialog.h eventdialog.cpp eventdialog.ui
        calendarwindow.h calendarwindow.cpp calendarwindow.ui
        dbmanager.h dbmanager.cpp
        init.sql
        user.h user.cpp
        tcpclient.h tcpclient.cpp
        calendarutils.h calendarutils.cpp
        todo.h todo.cpp
        attendance.h attendance.cpp
        permission.h permission.cpp
        protocol.h
        packetbuilder.h
        packetparser.h
        server.h server.cpp
        clienthandler.h clienthandler.cpp
)
# 리소스 파일이 있으면 추가 (나중에 추가할 때)
 if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc")
     list(APPEND PROJECT_SOURCES resources.qrc)
 endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(calendar
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        resources.qrc

    )
else()
    if(ANDROID)
        add_library(calendar SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(calendar
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(calendar PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(calendar PRIVATE Qt6::Core Qt6::Widgets)
target_link_libraries(calendar PRIVATE Qt6::Sql)
target_link_libraries(calendar PRIVATE Qt6::Core Qt6::Gui Qt6::Network)

# Network 라이브러리 링크 (TCP/IP 통신용)
if(TARGET Qt6::Network)
    target_link_libraries(calendar PRIVATE Qt6::Network)
endif()

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.calendar)
endif()
set_target_properties(calendar PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS calendar
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(calendar)
endif()
