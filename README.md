Adapter to add imgui to an existing vtkRenderWindow.

Provides two libraries:
- `imgui-vtk` using glfw3 and opengl3
- `vtkImguiAdapter` that accepts an already populated vtkRenderWindow (with renderer, interactor, and actors)
and permits mixing it with imgui components. It uses `imgui-vtk`

The dependencies needed are VTK and glfw3.

The libraries provide a vtkImguiAdapterConfig.cmake in build and install tree to facilitate integration with other libraries using CMake.

## Build example (bash)

```bash
mkdir imgui-vtk; cd imgui-vtk
git clone --recursive https://github.com/phcerdan/imgui-vtk src
mkdir build;
cmake -GNinja -DCMAKE_BUILD_TYPE=Release ../src
# Or specify dependencies location
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ../src -Dglfw3_DIR=/path/to -DVTK_DIR=/path/to
# It accepts building with BUILD_SHARED_LIBS ON or OFF.
```

## Screenshot from example.cpp
![image](https://user-images.githubusercontent.com/3021667/90887214-3b7eb700-e3b4-11ea-929f-ead3e7edb555.png)

## Acknowledgments:
Improved from work initiated in https://github.com/trlsmax/imgui-vtk
