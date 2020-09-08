cmake_minimum_required(VERSION 3.12)

project(vtkImguiAdapter-project
  VERSION 0.1.0
  LANGUAGES CXX C)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(GNUInstallDirs) # Define CMAKE_INSTALL_xxx: LIBDIR, INCLUDEDIR
set(vtkImguiAdapter_export_file "${PROJECT_BINARY_DIR}/vtkImguiAdapterTargets.cmake")

add_subdirectory(imgui)

set(vtk_components
  CommonCore
  CommonDataModel
  RenderingCore
  RenderingOpenGL2
  # RenderingContextOpenGL2
  RenderingVolumeOpenGL2
  InteractionStyle
  FiltersCore
  FiltersModeling
  FiltersSources
  )
find_package(VTK 9.0 REQUIRED COMPONENTS
        ${vtk_components}
        CONFIG)
message(STATUS "VTK_VERSION: ${VTK_VERSION}")
message(STATUS "VTK_LIBRARIES: ${VTK_LIBRARIES}")

if(NOT DEFINED SDL2_FOUND)
  message(STATUS "- VTK hasn't been compiled with SDL2 Support. Using needed files from this project instead.")
  message(STATUS "- You can compile VTK with VTK_USE_SDL2=ON")
endif()
set(_vtk_supports_sdl2 OFF)
if(SDL2_FOUND)
  set(_vtk_supports_sdl2 ON)
endif()


set(vtkImguiAdapter_headers
    vtkImguiSDL2RenderWindowInteractor.h
    vtkImguiSDL2OpenGLRenderWindow.h
  )
if(NOT _vtk_supports_sdl2)
  list(APPEND vtkImguiAdapter_headers
    vtkSDL2OpenGLRenderWindow.h
    vtkSDL2RenderWindowInteractor.h
    )
endif()

set(vtkImguiAdapter_sources
    vtkImguiSDL2RenderWindowInteractor.cxx
    vtkImguiSDL2OpenGLRenderWindow.cxx
    )

if(NOT _vtk_supports_sdl2)
  list(APPEND vtkImguiAdapter_sources
      vtkSDL2OpenGLRenderWindow.cxx
      vtkSDL2RenderWindowInteractor.cxx
      )
endif()

add_library(vtkImguiAdapter ${vtkImguiAdapter_sources})
target_link_libraries(vtkImguiAdapter PUBLIC
        ${EXTRA_LIBS}
        ${VTK_LIBRARIES}
        imgui-vtk
)
add_library(vtkImguiAdapter::vtkImguiAdapter ALIAS vtkImguiAdapter)
target_include_directories(vtkImguiAdapter PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

# SDL2 example
add_executable(example-imgui-vtk-sdl example-imgui-vtk-sdl.cpp)
target_link_libraries(example-imgui-vtk-sdl vtkImguiAdapter)
vtk_module_autoinit(
  TARGETS example-imgui-vtk-sdl
  MODULES ${VTK_LIBRARIES}
  )

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
