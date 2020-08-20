cmake_minimum_required(VERSION 3.6)
project(imgui-vtk)

add_subdirectory(imgui)
add_executable(imgui-vtk main.cpp vtk.cpp)


find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

target_link_libraries(
        imgui-vtk
        ${EXTRA_LIBS}
        ${VTK_LIBRARIES}
        imgui
        glfw
)
