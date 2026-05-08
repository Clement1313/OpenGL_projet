cmake_minimum_required(VERSION 3.12)
project(OpenGLTriangle LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)

add_executable(OpenGLTriangle
    main.cc
    src/App.cc
    src/FireRenderer.cc
)

target_include_directories(OpenGLTriangle PRIVATE include)

target_link_libraries(OpenGLTriangle PRIVATE
    OpenGL::GL
    GLEW::GLEW
    GLUT::GLUT
)
