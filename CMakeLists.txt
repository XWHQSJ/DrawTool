cmake_minimum_required(VERSION 3.14)
project(DrawTool VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Suppress deprecated GLUT warnings on macOS
if(APPLE)
    add_compile_options(-Wno-deprecated-declarations)
endif()

# ── Main executable ──────────────────────────────────────────────

add_executable(drawtool openGL5/main.cpp)

target_include_directories(drawtool PRIVATE ${CMAKE_SOURCE_DIR}/openGL5)

if(APPLE)
    target_link_libraries(drawtool
        "-framework OpenGL"
        "-framework GLUT"
    )
elseif(UNIX)
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)
    target_link_libraries(drawtool OpenGL::GL OpenGL::GLU GLUT::GLUT)
elseif(WIN32)
    target_link_libraries(drawtool opengl32 glu32 freeglut)
endif()

# ── Tests ────────────────────────────────────────────────────────

option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    enable_testing()
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
    )
    # Prevent gtest from overriding compiler/linker settings on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(geometry_test tests/geometry_test.cpp)
    target_include_directories(geometry_test PRIVATE ${CMAKE_SOURCE_DIR}/openGL5)
    target_link_libraries(geometry_test GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(geometry_test)
endif()
