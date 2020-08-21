set(imgui_headers_sources
        imgui/imgui.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_demo.cpp
        imgui/examples/imgui_impl_glfw.cpp
        imgui/examples/imgui_impl_opengl3.cpp
        )
set(imgui_headers_common
        imgui/imconfig.h
        imgui/imgui.h
        imgui/imgui_internal.h
        imgui/imstb_rectpack.h
        imgui/imstb_textedit.h
        imgui/imstb_truetype.h
        )
set(imgui_headers_glfw_opengl
        imgui/examples/imgui_impl_opengl3.h
        imgui/examples/imgui_impl_glfw.h
        )
set(imgui_headers_all
    ${imgui_headers_common}
    ${imgui_headers_glfw_opengl}
    )

if (APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(OpenGL_LIBRARY OpenGL)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    set(EXTRA_LIBS ${COCOA_LIBRARY} ${OpenGL_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY})
endif (APPLE)
# glfw
# Provide glfw3_DIR
find_package(glfw3 CONFIG REQUIRED)
# opengl
find_package(OpenGL REQUIRED)

add_library(imgui-vtk STATIC ${imgui_headers_sources})
add_library(vtkImguiAdapter::imgui-vtk ALIAS imgui-vtk)
target_link_libraries(imgui-vtk glfw)
target_link_libraries(imgui-vtk ${OPENGL_LIBRARIES})
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
