cmake_minimum_required(VERSION 3.20)
project(voxel_game)

set(CMAKE_CXX_STANDARD 23)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED) # nuovo!

add_executable(voxel_game
        src/main.cpp
        src/shader.cpp
        src/camera.cpp
        src/debug_ui.cpp
        src/debug_ui.cpp
        src/debug_ui.h  # nuovo!
)

target_link_libraries(voxel_game PRIVATE
        glfw
        glad::glad
        glm::glm
        imgui::imgui  # nuovo!
)

target_include_directories(voxel_game PRIVATE src)