cmake_minimum_required(VERSION 3.10.2)
project(mineworld)

set(CMAKE_CXX_STANDARD 11)

# glfw
add_subdirectory(deps/glfw)

# glew
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

# SOIL
add_subdirectory(deps/Simple-OpenGL-Image-Library)

# rapidjson & glm & SOIL
include_directories(deps/rapidjson/include deps/glm deps/Simple-OpenGL-Image-Library/src)

add_executable(mineworld src/block.cpp src/cell.cpp src/config.cpp src/entity.cpp src/glbuffer.cpp src/handler.cpp src/hitbox.cpp src/image.cpp src/main.cpp src/mineworld.cpp src/model.cpp src/shader.cpp src/shape.cpp src/terminal.cpp src/texture.cpp src/timer.cpp src/types.cpp src/util.cpp src/window.cpp src/worker.cpp src/world.cpp)
target_link_libraries(mineworld glfw SOIL GLEW::GLEW)
