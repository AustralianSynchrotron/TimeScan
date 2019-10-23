cmake_minimum_required(VERSION 3.0)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

project(TimeScan
        VERSION 1.0)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
include_directories(${Qt5Widgets_INCLUDE_DIRS})
find_package(Qt5 COMPONENTS PrintSupport REQUIRED)
include_directories(${Qt5PrintSupport_INCLUDE_DIRS})

find_package(QwtQt5 6.0 REQUIRED)
include_directories(${QWT_INCLUDE_DIRS})

find_path(QEPICSPV_INC qtpv.h)
if(NOT QEPICSPV_INC)
  message(FATAL_ERROR ": Can't find folder containing qtpv.h")
endif()
include_directories(${QEPICSPV_INC})


add_subdirectory(lib)
add_subdirectory(bin)
