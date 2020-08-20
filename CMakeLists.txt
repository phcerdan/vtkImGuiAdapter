cmake_minimum_required(VERSION 3.6)
project(imgui-vtk)

add_subdirectory(imgui)
add_executable(imgui-vtk main.cpp vtkImguiAdapter.cpp)


find_package(VTK REQUIRED CONFIG)
include(${VTK_USE_FILE})

target_link_libraries(
        imgui-vtk
        ${EXTRA_LIBS}
        ${VTK_LIBRARIES}
        imgui
        glfw
)
