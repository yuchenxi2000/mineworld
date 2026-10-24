cmake_minimum_required(VERSION 3.15)

# Honor the selected MSVC runtime consistently in this project and subprojects.
cmake_policy(SET CMP0091 NEW)

project(mineworld)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
find_package(Threads REQUIRED)

# glew
# GLEW_CUSTOM_PATH can be set to use a pre-built universal GLEW (macOS packaging).
# Otherwise the standard find_package path is used.
if(DEFINED GLEW_CUSTOM_PATH)
    add_library(GLEW::GLEW STATIC IMPORTED)
    set_target_properties(GLEW::GLEW PROPERTIES
        IMPORTED_LOCATION "${GLEW_CUSTOM_PATH}/lib/libGLEW.a"
        INTERFACE_INCLUDE_DIRECTORIES "${GLEW_CUSTOM_PATH}/include"
    )
    set_property(TARGET GLEW::GLEW PROPERTY INTERFACE_COMPILE_DEFINITIONS GLEW_STATIC)
else()
    option(GLEW_SHARED "Use shared GLEW library (DLL/dylib) instead of static" OFF)
    if(MSVC)
        # FindGLEW first tries config packages, which can select MSYS2 even when
        # GLEW_ROOT names an MSVC package. Import the explicit Windows package.
        set(GLEW_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/deps/glew-2.3.1" CACHE PATH "GLEW Windows binary package directory")
        if(NOT EXISTS "${GLEW_ROOT}/include/GL/glew.h")
            message(FATAL_ERROR "GLEW_ROOT is missing include/GL/glew.h: ${GLEW_ROOT}. Extract the complete GLEW Windows package (include, lib, bin). Do not use MSYS2 headers with MSVC.")
        endif()
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(_glew_arch x64)
        else()
            set(_glew_arch Win32)
        endif()
        find_package(OpenGL REQUIRED)
        if(GLEW_SHARED)
            set(_glew_library "${GLEW_ROOT}/lib/Release/${_glew_arch}/glew32.lib")
            set(_glew_dll "${GLEW_ROOT}/bin/Release/${_glew_arch}/glew32.dll")
            if(NOT EXISTS "${_glew_dll}")
                message(FATAL_ERROR "GLEW DLL not found: ${_glew_dll}")
            endif()
            add_library(GLEW::GLEW SHARED IMPORTED)
            set_target_properties(GLEW::GLEW PROPERTIES IMPORTED_IMPLIB "${_glew_library}" IMPORTED_LOCATION "${_glew_dll}")
        else()
            set(_glew_library "${GLEW_ROOT}/lib/Release/${_glew_arch}/glew32s.lib")
            add_library(GLEW::GLEW STATIC IMPORTED)
            set_target_properties(GLEW::GLEW PROPERTIES IMPORTED_LOCATION "${_glew_library}" INTERFACE_COMPILE_DEFINITIONS GLEW_STATIC)
            # Official prebuilt static GLEW uses /MT, including its Release-only
            # CRT. Match it for the game, GLFW and SOIL to avoid mixed runtimes.
            set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded)
        endif()
        if(NOT EXISTS "${_glew_library}")
            message(FATAL_ERROR "GLEW MSVC library not found: ${_glew_library}")
        endif()
        set_target_properties(GLEW::GLEW PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${GLEW_ROOT}/include"
            INTERFACE_LINK_LIBRARIES OpenGL::GL)
        message(STATUS "GLEW (MSVC ${_glew_arch}): ${_glew_library}")
    elseif(GLEW_SHARED OR APPLE)
        set(GLEW_USE_STATIC_LIBS OFF)
    else()
        set(GLEW_USE_STATIC_LIBS ON)
    endif()
    if(NOT MSVC)
        find_package(GLEW REQUIRED)
    endif()
endif()

# Link GLEW only to targets using it. Its include path must never leak into SOIL.
add_subdirectory(deps/glfw)

# SOIL (upstream cmake_minimum_required is 2.6, need policy override)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
add_subdirectory(deps/Simple-OpenGL-Image-Library)

# rapidjson & glm & SOIL
include_directories(SYSTEM deps/rapidjson/include deps/glm deps/Simple-OpenGL-Image-Library/src)

add_library(mineworld_core STATIC src/block.cpp src/cell.cpp src/config.cpp src/entity.cpp src/handler.cpp src/hitbox.cpp src/model.cpp src/shader.cpp src/shape.cpp src/terminal.cpp src/texture.cpp src/timer.cpp src/util.cpp src/window.cpp src/worker.cpp src/world.cpp)
target_include_directories(mineworld_core PUBLIC src)
target_link_libraries(mineworld_core PUBLIC glfw SOIL GLEW::GLEW Threads::Threads)
if(MSVC)
    target_compile_options(mineworld_core PRIVATE /W4)
else()
    target_compile_options(mineworld_core PRIVATE -Wall -Wextra -Wpedantic)
endif()
add_executable(mineworld src/main.cpp)
target_link_libraries(mineworld PRIVATE mineworld_core)

option(BUILD_TESTING "Build regression tests" OFF)
include(CTest)
if(BUILD_TESTING)
    add_executable(mineworld_tests tests/regression.cpp)
    target_link_libraries(mineworld_tests PRIVATE mineworld_core)
    if(MINGW)
        target_link_options(mineworld_tests PRIVATE -static)
    endif()
    add_test(NAME mineworld_regression COMMAND mineworld_tests)
    set_tests_properties(mineworld_regression PROPERTIES WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/res" TIMEOUT 30)
    option(MINEWORLD_GL_TESTS "Run smoke tests with a hidden OpenGL window" OFF)
    if(MINEWORLD_GL_TESTS)
        add_executable(mineworld_gl_smoke tests/opengl_smoke.cpp)
        target_link_libraries(mineworld_gl_smoke PRIVATE mineworld_core)
        if(MINGW)
            target_link_options(mineworld_gl_smoke PRIVATE -static)
        endif()
        add_test(NAME mineworld_gl_smoke COMMAND mineworld_gl_smoke)
        set_tests_properties(mineworld_gl_smoke PROPERTIES WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/res" TIMEOUT 30)
    endif()
endif()

# MinGW (MSYS2 etc.) must link runtime statically, otherwise users
# without MSYS2 installed will miss libgcc/libstdc++/libwinpthread DLLs.
# MSVC uses the runtime selected above to match the GLEW package.
if(MINGW)
    target_link_options(mineworld PRIVATE -static)
endif()

if(MSVC AND GLEW_SHARED)
    foreach(_target mineworld mineworld_tests mineworld_gl_smoke)
        if(TARGET ${_target})
            add_custom_command(TARGET ${_target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "$<TARGET_FILE:GLEW::GLEW>" "$<TARGET_FILE_DIR:${_target}>")
        endif()
    endforeach()
endif()

# macOS: packaging target — bundles GLEW dylib and res/ for GitHub Releases.
# Usage: cmake --build build --target package
if(APPLE)
    add_custom_target(package
        COMMAND bash "${CMAKE_SOURCE_DIR}/package_macos.sh" "${CMAKE_BINARY_DIR}" "${CMAKE_SOURCE_DIR}"
        DEPENDS mineworld
    )
endif()
