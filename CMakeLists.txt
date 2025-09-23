cmake_minimum_required(VERSION 3.10)
project(targets)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)

add_executable(targets targets.cpp game.cpp engine.cpp gameobject.cpp)

target_include_directories(targets PRIVATE
    ${SDL2_INCLUDE_DIRS}
)

# Linka explicitamente SDL2, SDL2_image e SDL2_mixer
target_link_libraries(targets
    ${SDL2_LIBRARIES}
    SDL2_image
    SDL2_mixer
)
