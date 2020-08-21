#pragma once
#include <vtk_glew.h>
#include <GLFW/glfw3.h>

#include <vtkSmartPointer.h>
#include <vtkGenericOpenGLRenderWindow.h>

class vtkGenericOpenGLRenderWindow;

class vtkImguiAdapter
{
public:
    vtkImguiAdapter();
    ~vtkImguiAdapter();
    void UpdateSize(unsigned int w, unsigned int h);
    void Render(void);
    void MouseButtonCallback(double xpos, double ypos, int button, int action, bool ctrl, bool shift, bool dclick);
    void MousePositionCallback(double xpos, double ypos, bool ctrl, bool shift);
    void MouseWheelCallback(double xoffset, double yoffset);
public:
    // Passes mouse to VTK and IMGUI
    static void glfw_mouse_wheel(GLFWwindow* window, double xoffset, double yoffset);
    static void glfw_mouse_position(GLFWwindow* window, double xpos, double ypos);
    static void glfw_mouse_button(GLFWwindow* window, int button, int action, int mods);
    static void glfw_window_size_callback(GLFWwindow *window, int width, int height);

public:
    void SetRenderWindow(vtkGenericOpenGLRenderWindow *renderWindow);
    vtkGenericOpenGLRenderWindow* GetRenderWindow();

private:
    bool m_IsInitiated;
    GLuint m_fbo;
    GLuint m_tex;
    GLuint m_rbo;
    unsigned int m_WindowWidth;
    unsigned int m_WindowHeight;

protected:
    vtkGenericOpenGLRenderWindow * m_vtkRenderWindow;
};

