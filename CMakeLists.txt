cmake_minimum_required(VERSION 3.12)

project(vtkImguiAdapter-project
  VERSION 0.1.0
  LANGUAGES CXX C)

option(vtkImguiAdapter_USE_GLFW "Use imgui glfw" ON)
option(vtkImguiAdapter_USE_SDL "Use imgui sdl" ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(GNUInstallDirs) # Define CMAKE_INSTALL_xxx: LIBDIR, INCLUDEDIR
set(vtkImguiAdapter_export_file "${PROJECT_BINARY_DIR}/vtkImguiAdapterTargets.cmake")

add_subdirectory(imgui)

find_package(VTK REQUIRED CONFIG)
message(STATUS "VTK_VERSION: ${VTK_VERSION}")
if(${VTK_VERSION} VERSION_GREATER_EQUAL "9.0")
  set(_vtk_is_modern ON)
else()
  set(_vtk_is_modern OFF)
endif()

if(_vtk_is_modern)
    set(vtk_components
      CommonCore
      CommonDataModel
      RenderingCore
      RenderingOpenGL2
      RenderingContextOpenGL2
      RenderingVolumeOpenGL2
      InteractionStyle
      FiltersCore
      FiltersModeling
      FiltersSources
      )
else()
  set(vtk_components
    vtkCommonCore
    vtkCommonDataModel
    # avoid AUTOINIT_redefined at build time,
    vtkRenderingCore
    vtkRenderingOpenGL2 #  vtkRenderer, vtkRenderWindow not found at run time. Includes SDL2 if VTK_USE_SDL2
    vtkRenderingContextOpenGL2 # avoid vtkContextDevice2D not found at run time
    vtkRenderingVolumeOpenGL2 # For volume rendering SmartVolumeMapper
    vtkInteractionStyle
    )
endif()

find_package(VTK REQUIRED COMPONENTS
        ${vtk_components}
        CONFIG)
if(NOT _vtk_is_modern)
  include(${VTK_USE_FILE})
endif()

if(_vtk_is_modern)
  if(NOT DEFINED SDL2_FOUND)
    message(STATUS "VTK hasn't been compiled with SDL2 Support.")
  endif()
  if(SDL2_FOUND)
    set(_vtk_supports_sdl2 ON)
  endif()
endif()
message(STATUS "VTK_LIBRARIES: ${VTK_LIBRARIES}")


set(vtkImguiAdapter_headers)
if(vtkImguiAdapter_USE_SDL)
  list(APPEND vtkImguiAdapter_headers
    vtkImguiSDL2RenderWindowInteractor.h
        )
endif()
if(vtkImguiAdapter_USE_GLFW)
  list(APPEND vtkImguiAdapter_headers
    vtkImguiGLFWAdapter.h
        )
endif()
if(NOT _vtk_supports_sdl2 AND vtkImguiAdapter_USE_SDL)
  list(APPEND vtkImguiAdapter_headers
    vtkSDL2OpenGLRenderWindow.h
    vtkSDL2RenderWindowInteractor.h
    )
endif()

set(vtkImguiAdapter_sources)
if(vtkImguiAdapter_USE_SDL)
  list(APPEND vtkImguiAdapter_sources
    vtkImguiSDL2RenderWindowInteractor.cxx
        )
endif()
if(vtkImguiAdapter_USE_GLFW)
  list(APPEND vtkImguiAdapter_sources
    vtkImguiGLFWAdapter.cpp
        )
endif()

if(NOT _vtk_supports_sdl2 AND vtkImguiAdapter_USE_SDL)
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

# GLFW example
if(vtkImguiAdapter_USE_GLFW)
  add_executable(example_glfw example.cpp)
  target_link_libraries(example_glfw vtkImguiAdapter)
  if(_vtk_is_modern)
    vtk_module_autoinit(
      TARGETS example_glfw
      MODULES ${VTK_LIBRARIES}
      )
  endif()
endif()

# SDL2 example
if(vtkImguiAdapter_USE_SDL)
  add_executable(vtk_sdl2 vtk_sdl2.cpp)
  target_link_libraries(vtk_sdl2 vtkImguiAdapter)
  if(_vtk_is_modern)
    vtk_module_autoinit(
      TARGETS vtk_sdl2
      MODULES ${VTK_LIBRARIES}
      )
  endif()
endif()

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
