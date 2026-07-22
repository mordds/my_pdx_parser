cmake_minimum_required(VERSION 3.16)

project(pdx_gui VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_PREFIX_PATH "D:/msys2/mingw64")
find_library(WINDOWSAPP_LIB windowsapp REQUIRED)
find_library(RUNTIMEOBJECT_LIB runtimeobject REQUIRED)
find_library(USER32_LIB user32 REQUIRED)
set(CMAKE_PREFIX_PATH "D:/static_qt/6.9.1")
add_compile_options(-Werror -O2)
add_link_options(-static)


find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core LinguistTools)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets LinguistTools)
find_library(Qt6Core2 Qt6Core REQUIRED)

BISON_TARGET(MyParser test.y ${CMAKE_CURRENT_BINARY_DIR}/y.tab.cpp
             DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/y.tab.h
             COMPILE_FLAGS "-vdty")

FLEX_TARGET(MyLexer pdx.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)


add_flex_bison_dependency(MyLexer MyParser)

file(GLOB COMMON_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/utils/*.cpp")

list(REMOVE_ITEM COMMON_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/test.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/console.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/b.cpp"
)

set(TS_FILES pdx_gui_zh_CN.ts)

set(PROJECT_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/gui/main.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/gui/mainwindow.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/gui/mainwindow.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/gui/mainwindow.ui"
        ${COMMON_SRC}
        ${BISON_MyParser_OUTPUTS}
        ${FLEX_MyLexer_OUTPUTS}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(pdx_gui
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET pdx_gui APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
        add_executable(pdx_gui
            ${PROJECT_SOURCES}
        )

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()
message(WARNING "${USER32_LIB} ${WINDOWSAPP_LIB} ${RUNTIMEOBJECT_LIB} ${QT6} ${CMAKE_PREFIX_PATH}/plugins/platforms/libqwindows.a")
target_link_libraries(pdx_gui PRIVATE ${CMAKE_PREFIX_PATH}/plugins/platforms/libqwindows.a zstd Qt6::Core Qt6::Gui Qt6::Widgets d3d9 setupapi shlwapi dwmapi winmm wtsapi32 imm32 shcore
    runtimeobject windowsapp
    )

set(_qt_libs "")
foreach(_lib IN LISTS _libs)
    if(TARGET ${_lib})
        # 如果它是 CMake 目标（例如 Qt6::Core），取其文件路径
        get_target_property(_lib_file ${_lib} LOCATION)
        if(_lib_file)
            list(APPEND _qt_libs "${_lib_file}")
        endif()
    elseif(_lib MATCHES "libqwindows\\.a$" OR _lib MATCHES "libQt6")
        # 如果是直接给出的插件路径
        list(APPEND _qt_libs "${_lib}")
    endif()
endforeach()

# 用 start-group 包裹这些库，并替换原有的链接方式
target_link_options(pdx_gui PRIVATE
    -Wl,--start-group
    ${_qt_libs}
    -Wl,--end-group
)

target_include_directories(pdx_gui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.pdx_gui)
endif()
set_target_properties(pdx_gui PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS pdx_gui
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(pdx_gui)
endif()
