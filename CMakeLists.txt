cmake_minimum_required(VERSION 3.6)

project(vtkImguiAdapter-project
  VERSION 0.1.0
  LANGUAGES CXX C)


set(CMAKE_POSITION_INDEPENDENT_CODE ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(GNUInstallDirs) # Define CMAKE_INSTALL_xxx: LIBDIR, INCLUDEDIR
set(vtkImguiAdapter_export_file "${PROJECT_BINARY_DIR}/vtkImguiAdapterTargets.cmake")

add_subdirectory(imgui)

set(vtk_components
    vtkInteractionStyle
    vtkCommonCore
    vtkCommonDataModel
    # avoid AUTOINIT_redefined at build time,
    vtkRenderingCore
    vtkRenderingOpenGL2 #  vtkRenderer, vtkRenderWindow not found at run time
    vtkRenderingContextOpenGL2 # avoid vtkContextDevice2D not found at run time
    vtkRenderingVolumeOpenGL2 # For volume rendering SmartVolumeMapper
    )

find_package(VTK REQUIRED COMPONENTS
        ${vtk_components}
        CONFIG)
include(${VTK_USE_FILE})

set(vtkImguiAdapter_headers
        vtkImguiAdapter.h
        )

add_library(vtkImguiAdapter vtkImguiAdapter.cpp)
target_link_libraries(vtkImguiAdapter
        ${EXTRA_LIBS}
        ${VTK_LIBRARIES}
        imgui-vtk
        glfw
)
add_library(vtkImguiAdapter::vtkImguiAdapter ALIAS vtkImguiAdapter)
add_executable(imgui-vtk-example example.cpp)
target_link_libraries(imgui-vtk-example vtkImguiAdapter)

install(TARGETS vtkImguiAdapter
        EXPORT vtkImguiAdapterTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtime
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT runtime
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT development
        )

install(FILES ${vtkImguiAdapter_headers}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/vtkImguiAdapter)

# export to the build tree
export( TARGETS vtkImguiAdapter
  NAMESPACE vtkImguiAdapter::
  APPEND FILE ${vtkImguiAdapter_export_file})

# INSTALL
set(install_cmake_dir "${CMAKE_INSTALL_LIBDIR}/cmake/vtkImguiAdapter")

install (EXPORT vtkImguiAdapterTargets
  NAMESPACE vtkImguiAdapter::
  DESTINATION ${install_cmake_dir} )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/vtkImguiAdapterConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/vtkImguiAdapterConfigVersion.cmake
              DESTINATION ${install_cmake_dir} )

include(CMakePackageConfigHelpers)

write_basic_package_version_file(vtkImguiAdapterConfigVersion.cmake
  VERSION ${vtkImguiAdapter_VERSION}
  COMPATIBILITY SameMajorVersion)

# Build tree
set(vtkImguiAdapter_targets_file ${vtkImguiAdapter_export_file})
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/vtkImguiAdapterConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/vtkImguiAdapterConfig.cmake
  INSTALL_DESTINATION ${install_cmake_dir}
  PATH_VARS vtkImguiAdapter_targets_file
  NO_CHECK_REQUIRED_COMPONENTS_MACRO # vtkImguiAdapter does not provide components
  INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}
  )

# Install tree
set(vtkImguiAdapter_targets_file ${CMAKE_INSTALL_PREFIX}/${install_cmake_dir}/vtkImguiAdapterTargets.cmake)
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/vtkImguiAdapterConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/vtkImguiAdapterConfig.cmake
  INSTALL_DESTINATION ${install_cmake_dir}
  PATH_VARS vtkImguiAdapter_targets_file
  NO_CHECK_REQUIRED_COMPONENTS_MACRO # vtkImguiAdapter does not provide components
  )

# Add custom target to only install component: runtime (libraries)
add_custom_target(vtkImguiAdapter-install-runtime
  ${CMAKE_COMMAND}
  -DCMAKE_INSTALL_COMPONENT=runtime
  -P "${PROJECT_BINARY_DIR}/cmake_install.cmake"
  DEPENDS ${SG_LIBRARIES}
  )
add_dependencies(vtkImguiAdapter-install-runtime vtkImguiAdapter)
