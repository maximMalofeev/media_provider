project(image_provider_test LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
enable_testing()

add_executable(tst_image_provider tst_image_provider.cpp)
add_test(tst_image_provider COMMAND tst_image_provider)
target_link_libraries(tst_image_provider PRIVATE Qt5::Test media_provider)

add_executable(tst_video_provider tst_video_provider.cpp)
add_test(tst_video_provider COMMAND tst_video_provider)
target_link_libraries(tst_video_provider PRIVATE Qt5::Test media_provider)

add_executable(tst_rtsp_provider tst_rtsp_provider.cpp)
add_test(tst_rtsp_provider COMMAND tst_rtsp_provider)
target_link_libraries(tst_rtsp_provider PRIVATE Qt5::Test media_provider)

add_executable(tst_sapera_provider tst_sapera_provider.cpp)
add_test(tst_sapera_provider COMMAND tst_sapera_provider)
target_link_libraries(tst_sapera_provider PRIVATE Qt5::Test media_provider)
