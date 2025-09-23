cmake_minimum_required(VERSION 3.10)
project(targets)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)

# targets.cpp na raiz, outros arquivos na pasta engine
add_executable(targets 
    targets.cpp
    game.cpp
    engine/engine.cpp 
    engine/gameobject.cpp
)

target_include_directories(targets PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/engine  # Para includes entre arquivos da engine
)

target_link_libraries(targets
    ${SDL2_LIBRARIES}
    SDL2_image
    SDL2_mixer
)