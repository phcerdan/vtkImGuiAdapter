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

add_library(imgui STATIC ${imgui_headers_sources})
target_link_libraries(imgui glfw)
target_link_libraries(imgui ${OPENGL_LIBRARIES})
target_include_directories(imgui PUBLIC "imgui")
target_include_directories(imgui PUBLIC "imgui/examples")
