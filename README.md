Adapter to add imgui to an existing vtkRenderWindow.

Provides two libraries:
- `imgui-vtk` a portion of imgui, with common headers and headers using sdl2 and opengl3.
- `vtkImguiAdapter` that accepts an already populated vtkRenderWindow (with renderer, interactor, and actors)
and permits mixing it with imgui components. It links to `imgui-vtk`.

The dependencies needed are VTK and OpenGL.
If VTK has been compiled with VTK_USE_SDL2, we don't need anything more.
Otherwise, files needed from VTK SDL2 module will be compiled here, and we will require SDL2 and GLEW as well.
This is to avoid making you rebuild VTK with VTK_USE_SDL2=ON.

The libraries provide a vtkImguiAdapterConfig.cmake in build and install tree to facilitate integration with other libraries using CMake.

## Build example (bash)

```bash
mkdir vtkImguiAdapter; cd vtkImguiAdapter
git clone --recursive https://github.com/phcerdan/vtkImguiAdapter src
mkdir build;
cmake -GNinja -DCMAKE_BUILD_TYPE=Release ../src
# Or specify dependencies location
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ../src -DVTK_DIR=/path/to
# It accepts building with BUILD_SHARED_LIBS ON or OFF.
```

## Screenshot from vtk_sdl2.cpp
![vtkImguiAdapter_example](https://user-images.githubusercontent.com/3021667/90983593-9c9eba00-e56f-11ea-9145-07088499e1eb.gif)

## Acknowledgments:
Improved from example initiated in https://github.com/trlsmax/imgui-vtk.
We use sdl2 instead of glfw to take adventage of VTK's vtkSDL2OpenGLRenderWindow and Interactor. 
These VTK classes allow a better interaction between imgui and vtk.
