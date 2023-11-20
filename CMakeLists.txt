cmake_minimum_required(VERSION 3.0.0)
project(yolo_and_gst VERSION 0.1.0 LANGUAGES C CXX)

set(FONT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/res/arial.ttf")
set(FONT_SIZE 18)
set(YOLOV3_PARAM_FILE "${CMAKE_CURRENT_SOURCE_DIR}/res/mobilenetv2_yolov3.param")
set(YOLOV3_BIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/res/mobilenetv2_yolov3.bin")

find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0 gstreamer-base-1.0 gstreamer-video-1.0)
pkg_check_modules(FT REQUIRED freetype2)

message("GST_INCLUDE_DIRS ${GST_INCLUDE_DIRS}")
message("GST_LIBRARY_DIRS ${GST_LIBRARY_DIRS}")
message("GST_LIBRARIES ${GST_LIBRARIES}")

message("FT_INCLUDE_DIRS ${FT_INCLUDE_DIRS}")
message("FT_LIBRARY_DIRS ${FT_LIBRARY_DIRS}")
message("FT_LIBRARIES ${FT_LIBRARIES}")

include_directories(${GST_INCLUDE_DIRS} ${FT_INCLUDE_DIRS})
link_directories(${GST_LIBRARY_DIRS} ${FT_LIBRARY_DIRS})

set(PREV_BUILD_TYPE "Release")
if(DEFINED CMAKE_BUILD_TYPE)
    set(PREV_BUILD_TYPE ${CMAKE_BUILD_TYPE})
endif()
set(CMAKE_BUILD_TYPE "Release")

option(NCNN_SHARED_LIB "shared library support" ON)
option(NCNN_OPENMP "openmp support" OFF)
option(NCNN_INSTALL_SDK "install ncnn library and headers" OFF)
option(NCNN_PLATFORM_API "build with platform api candy" OFF)
option(NCNN_PIXEL_ROTATE "rotate image pixel orientation" OFF)
option(NCNN_PIXEL_AFFINE "warp affine image pixel" OFF)
option(NCNN_PIXEL_DRAWING "draw basic figure and text" OFF)
option(NCNN_SIMPLEVK "minimal in-house vulkan loader" OFF)
option(NCNN_BUILD_BENCHMARK "build benchmark" OFF)
option(NCNN_BUILD_EXAMPLES "build examples" OFF)
option(NCNN_BUILD_TOOLS "build tools" OFF)
add_subdirectory(ncnn)

set(CMAKE_BUILD_TYPE ${PREV_BUILD_TYPE})

set(NCNN_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/ncnn/src" "${CMAKE_CURRENT_BINARY_DIR}/ncnn/src")
message("NCNN_INCLUDE_DIRS ${NCNN_INCLUDE_DIRS}")
include_directories(${NCNN_INCLUDE_DIRS})

include_directories(src)
add_compile_definitions(FONT_PATH="${FONT_PATH}" FONT_SIZE=${FONT_SIZE} YOLOV3_PARAM_FILE="${YOLOV3_PARAM_FILE}" YOLOV3_BIN_FILE="${YOLOV3_BIN_FILE}")
add_executable(${PROJECT_NAME} src/main.cpp src/utils.cpp)

target_link_libraries(${PROJECT_NAME} ${GST_LIBRARIES} ${FT_LIBRARIES} ncnn)
