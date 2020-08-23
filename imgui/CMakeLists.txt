set(imgui_sources
        imgui/imgui.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_demo.cpp
        imgui/examples/imgui_impl_opengl3.cpp
        )
if(vtkImguiAdapter_USE_GLFW)
  list(APPEND imgui_sources imgui/examples/imgui_impl_glfw.cpp)
endif()
if(vtkImguiAdapter_USE_SDL)
  list(APPEND imgui_sources imgui/examples/imgui_impl_sdl.cpp)
endif()
set(imgui_headers_common
        imgui/imconfig.h
        imgui/imgui.h
        imgui/imgui_internal.h
        imgui/imstb_rectpack.h
        imgui/imstb_textedit.h
        imgui/imstb_truetype.h
        )
set(imgui_headers_opengl
        imgui/examples/imgui_impl_opengl3.h
        )
set(imgui_headers_glfw_glfw
        imgui/examples/imgui_impl_glfw.h
        )
set(imgui_headers_glfw_sdl
        imgui/examples/imgui_impl_sdl.h
        )
set(imgui_headers_all
    ${imgui_headers_common}
    ${imgui_headers_opengl}
    )
if(vtkImguiAdapter_USE_GLFW)
  list(APPEND imgui_headers_all ${imgui_headers_glfw})
endif()
if(vtkImguiAdapter_USE_SDL)
  list(APPEND imgui_headers_all ${imgui_headers_sdl})
endif()

if (APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(OpenGL_LIBRARY OpenGL)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    set(EXTRA_LIBS ${COCOA_LIBRARY} ${OpenGL_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY})
endif (APPLE)
# glfw
# Provide glfw3_DIR
if(vtkImguiAdapter_USE_GLFW)
  find_package(glfw3 CONFIG REQUIRED)
endif()
if(vtkImguiAdapter_USE_SDL)
    if(NOT _vtk_supports_sdl2)
      find_package(SDL2 CONFIG REQUIRED)
      find_package(GLEW REQUIRED)
    endif()
endif()
# opengl
find_package(OpenGL REQUIRED)

add_library(imgui-vtk ${imgui_sources})
add_library(vtkImguiAdapter::imgui-vtk ALIAS imgui-vtk)
target_link_libraries(imgui-vtk PUBLIC OpenGL::GL)
if(vtkImguiAdapter_USE_GLFW)
  target_link_libraries(imgui-vtk PUBLIC glfw)
endif()
if(vtkImguiAdapter_USE_SDL)
  if(NOT _vtk_supports_sdl2)
    target_link_libraries(imgui-vtk PUBLIC SDL2::SDL2)
    target_link_libraries(imgui-vtk PUBLIC GLEW::GLEW)
  endif()
endif()
target_include_directories(imgui-vtk PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imgui>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imgui/examples>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )


install(TARGETS imgui-vtk
        EXPORT vtkImguiAdapterTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtime
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT runtime
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT development
        )

install(FILES ${imgui_headers_all}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/vtkImguiAdapter)

# export to the build tree
export( TARGETS imgui-vtk
  NAMESPACE vtkImguiAdapter::
  APPEND FILE ${vtkImguiAdapter_export_file})
