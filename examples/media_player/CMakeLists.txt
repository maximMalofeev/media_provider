project(media_player)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(sources "src/main.cpp"
    "src/backend.h" "src/backend.cpp")

set(qrc "qml/qml.qrc")

add_executable(${PROJECT_NAME} ${sources} ${qrc})

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Quick media_provider)

