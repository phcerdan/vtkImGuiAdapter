set(imgui_sources
        imgui/imgui.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_demo.cpp
        imgui/backends/imgui_impl_opengl3.cpp
        imgui/backends/imgui_impl_sdl.cpp
        )
set(imgui_headers_common
        imgui/imconfig.h
        imgui/imgui.h
        imgui/imgui_internal.h
        imgui/imstb_rectpack.h
        imgui/imstb_textedit.h
        imgui/imstb_truetype.h
        )
set(imgui_headers_opengl
        imgui/backends/imgui_impl_opengl3.h
        )
set(imgui_headers_glfw_sdl
        imgui/backends/imgui_impl_sdl.h
        )
set(imgui_headers_all
    ${imgui_headers_common}
    ${imgui_headers_opengl}
    ${imgui_headers_sdl}
    )

if (APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(OpenGL_LIBRARY OpenGL)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    set(EXTRA_LIBS ${COCOA_LIBRARY} ${OpenGL_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY})
endif (APPLE)

if(NOT _vtk_supports_sdl2)
  find_package(SDL2 CONFIG REQUIRED)
  find_package(GLEW REQUIRED)
endif()

# opengl
find_package(OpenGL REQUIRED)

add_library(imgui-vtk ${imgui_sources})
add_library(vtkImguiAdapter::imgui-vtk ALIAS imgui-vtk)
target_link_libraries(imgui-vtk PUBLIC OpenGL::GL)
if(NOT _vtk_supports_sdl2)
  target_link_libraries(imgui-vtk PUBLIC SDL2::SDL2)
  target_link_libraries(imgui-vtk PUBLIC GLEW::GLEW)
endif()
target_include_directories(imgui-vtk PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imgui>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends>
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
