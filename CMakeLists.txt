cmake_minimum_required(VERSION 3.12)
project(OpenGLTriangle LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(IMGUI_ELEMENT
    external/imgui/imgui.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_widgets.cpp
        external/imgui/backends/imgui_impl_glut.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)

add_executable(OpenGLTriangle
    main.cc
    src/App.cc
    src/Controller.cc
    src/FireRenderer.cc
        src/TorchRenderer.cc
        src/TorchRenderer.cc
)
target_sources(OpenGLTriangle PRIVATE ${IMGUI_ELEMENT})
target_include_directories(OpenGLTriangle PRIVATE include src external/imgui external/imgui/backends)

target_link_libraries(OpenGLTriangle PRIVATE
    OpenGL::GL
    GLEW::GLEW
    GLUT::GLUT
)
