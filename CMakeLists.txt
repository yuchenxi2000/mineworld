cmake_minimum_required(VERSION 3.12)

project(mineworld)

set(CMAKE_CXX_STANDARD 17)

# glfw
add_subdirectory(deps/glfw)

# glew
# GLEW_CUSTOM_PATH can be set to use a pre-built universal GLEW (macOS packaging).
# Otherwise the standard find_package path is used.
if(DEFINED GLEW_CUSTOM_PATH)
    add_library(GLEW::GLEW STATIC IMPORTED)
    set_target_properties(GLEW::GLEW PROPERTIES
        IMPORTED_LOCATION "${GLEW_CUSTOM_PATH}/lib/libGLEW.a"
        INTERFACE_INCLUDE_DIRECTORIES "${GLEW_CUSTOM_PATH}/include"
    )
    add_definitions(-DGLEW_STATIC)
else()
    option(GLEW_SHARED "Use shared GLEW library (DLL/dylib) instead of static" OFF)
    if(GLEW_SHARED OR APPLE)
        set(GLEW_USE_STATIC_LIBS OFF)
    else()
        set(GLEW_USE_STATIC_LIBS ON)
    endif()
    find_package(GLEW REQUIRED)
    include_directories(${GLEW_INCLUDE_DIRS})
    if(GLEW_USE_STATIC_LIBS)
        add_definitions(-DGLEW_STATIC)
    endif()
endif()

# SOIL (upstream cmake_minimum_required is 2.6, need policy override)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
add_subdirectory(deps/Simple-OpenGL-Image-Library)

# rapidjson & glm & SOIL
include_directories(deps/rapidjson/include deps/glm deps/Simple-OpenGL-Image-Library/src)

add_executable(mineworld src/block.cpp src/cell.cpp src/config.cpp src/entity.cpp src/glbuffer.cpp src/handler.cpp src/hitbox.cpp src/image.cpp src/main.cpp src/mineworld.cpp src/model.cpp src/shader.cpp src/shape.cpp src/terminal.cpp src/texture.cpp src/timer.cpp src/types.cpp src/util.cpp src/window.cpp src/worker.cpp src/world.cpp)
target_link_libraries(mineworld glfw SOIL GLEW::GLEW)

# MinGW (MSYS2 etc.) must link runtime statically, otherwise users
# without MSYS2 installed will miss libgcc/libstdc++/libwinpthread DLLs.
# MSVC uses dynamic CRT by default — users can install VC Redist.
if(MINGW)
    target_link_options(mineworld PRIVATE -static)
endif()

# macOS: packaging target — bundles GLEW dylib and res/ for GitHub Releases.
# Usage: cmake --build build --target package
if(APPLE)
    add_custom_target(package
        COMMAND bash "${CMAKE_SOURCE_DIR}/package_macos.sh" "${CMAKE_BINARY_DIR}" "${CMAKE_SOURCE_DIR}"
        DEPENDS mineworld
    )
endif()
