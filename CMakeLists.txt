cmake_minimum_required(VERSION 3.10)

project(media_provider)

set (${PROJECT_NAME}_VERSION_MAJOR 1)
set (${PROJECT_NAME}_VERSION_MINOR 0)

find_package(Qt5 COMPONENTS Core Gui Widgets Quick Test Network Xml Multimedia Concurrent REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(BUILD_SHARED_LIBS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(sources "include/media_provider/media_provider.h"
    "src/media_provider.cpp"
    "src/file_based_provider.h" "src/file_based_provider.cpp"
    "src/image_provider.h" "src/image_provider.cpp"
    "src/video_provider.h" "src/video_provider.cpp"
    "src/rtsp_provider.h" "src/rtsp_provider.cpp"
    "src/image_resource.h" "src/image_resource.cpp"
    "src/qmediaplayer_based_resource.h" "src/qmediaplayer_based_resource.cpp"
    "src/video_resource.h" "src/video_resource.cpp"
    "src/rtsp_resource.h" "src/rtsp_resource.cpp"
    "src/image_stream.h" "src/image_stream.cpp"
    "src/video_stream.h" "src/video_stream.cpp"
    "src/rtsp_stream.h" "src/rtsp_stream.cpp"
    "src/video_surface.h" "src/video_surface.cpp")

if(WIN32)
    set(sources ${sources}
        "src/sapera_win_provider.h" "src/sapera_win_provider.cpp"
        "src/sapera_win_resource.h" "src/sapera_win_resource.cpp"
        "src/sapera_win_stream.h" "src/sapera_win_stream.cpp"
        "src/sapera_win_buffer_processing.h" "src/sapera_win_buffer_processing.cpp"
        "src/sapera_win_shortcuts.h" "src/sapera_win_shortcuts.cpp")
elseif(UNIX)
    set(sources ${sources}
        "src/sapera_lnx_provider.h" "src/sapera_lnx_provider.cpp"
        "src/sapera_lnx_resource.h" "src/sapera_lnx_resource.cpp"
        #"src/sapera_lnx_stream.h" "src/sapera_lnx_stream.cpp"
        )
endif()

add_library(${PROJECT_NAME} ${sources})

target_compile_definitions(${PROJECT_NAME} PRIVATE -D__MODULENAME__=${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_BINARY_DIR}" PUBLIC "include")
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Multimedia)

if(WIN32)
    add_compile_options(-MP$ENV{NUMBER_OF_PROCESSORS})
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DMEDIAPROVIDERLIB_LIBRARY)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ignore:4099")
    target_include_directories(${PROJECT_NAME} PRIVATE "$ENV{SAPERADIR}/Classes/Basic"
        PRIVATE "$ENV{SAPERADIR}/Include")
    target_link_libraries(${PROJECT_NAME} "$ENV{SAPERADIR}/Lib/Win32/SapClassBasic.lib")
elseif(UNIX)
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DMEDIAPROVIDERLIB_LIBRARY)
    target_include_directories(${PROJECT_NAME} PRIVATE "$ENV{GENICAM_ROOT_V3_0}/library/CPP/include" PRIVATE "/usr/dalsa/GigeV/include")
    target_link_libraries(${PROJECT_NAME} "$ENV{GENICAM_ROOT_V3_0}/bin/Linux64_x64/libGenApi_gcc421_v3_0.so"
        "$ENV{GENICAM_ROOT_V3_0}/bin/Linux64_x64/libGCBase_gcc421_v3_0.so" GevApi)
endif()

add_subdirectory(examples)
add_subdirectory(test)
