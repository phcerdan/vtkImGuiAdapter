#include "vtkImguiAdapter.h"
#include <vtkGenericOpenGLRenderWindow.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <vtkGenericRenderWindowInteractor.h>

vtkImguiAdapter::vtkImguiAdapter()
    :m_IsInitiated(false)
    ,m_WindowWidth(0)
    ,m_WindowHeight(0)
    ,m_fbo(0)
    ,m_tex(0)
    ,m_rbo(0)
    ,m_vtkRenderWindow(nullptr)
{}

vtkImguiAdapter::~vtkImguiAdapter()
{
    glDeleteBuffers(1, &m_fbo);
    glDeleteBuffers(1, &m_rbo);
    glDeleteTextures(1, &m_tex);
}

void vtkImguiAdapter::SetRenderWindow(vtkGenericOpenGLRenderWindow *renderWindow)
{
    m_vtkRenderWindow = renderWindow;
}

vtkGenericOpenGLRenderWindow * vtkImguiAdapter::GetRenderWindow()
{
    return m_vtkRenderWindow;
}

void vtkImguiAdapter::glfw_mouse_wheel(GLFWwindow* window, double xoffset, double yoffset)
{
    if (ImGui::IsAnyWindowHovered())
        return;
    vtkImguiAdapter* this_ = static_cast<vtkImguiAdapter *>(glfwGetWindowUserPointer(window));
    this_->MouseWheelCallback(xoffset, yoffset);
}

void vtkImguiAdapter::glfw_mouse_position(GLFWwindow* window, double xpos, double ypos)
{
    if (ImGui::IsAnyWindowHovered())
        return;
    vtkImguiAdapter* this_ = static_cast<vtkImguiAdapter *>(glfwGetWindowUserPointer(window));
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        bool ctrl = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;
        bool shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
        this_->MousePositionCallback(xpos, ypos, ctrl, shift);
    }
}
void vtkImguiAdapter::glfw_mouse_button(GLFWwindow* window, int button, int action, int mods)
{
    if (ImGui::IsAnyWindowHovered())
        return;
    vtkImguiAdapter* this_ = static_cast<vtkImguiAdapter *>(glfwGetWindowUserPointer(window));
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    this_->MouseButtonCallback(xpos, ypos, button, action, mods == GLFW_MOD_CONTROL, mods == GLFW_MOD_SHIFT, false);
}

void vtkImguiAdapter::UpdateSize(unsigned int w, unsigned int h)
{
    if (w == m_WindowWidth && h == m_WindowHeight)
        return;

    if (w == 0 || h == 0)
        return;

    m_WindowWidth = w;
    m_WindowHeight = h;

    // resize the render window
    m_vtkRenderWindow->SetSize(m_WindowWidth, m_WindowHeight);
    m_vtkRenderWindow->FullScreenOn();
    m_vtkRenderWindow->OffScreenRenderingOn();
    //m_vtkRenderWindow->SetMultiSamples(0);
    m_vtkRenderWindow->Modified();
    // m_vtkRenderWindow->GetInteractor()->UpdateSize(m_WindowWidth, m_WindowHeight);
    // m_vtkRenderWindow->GetInteractor()->Modified();
    m_IsInitiated = false;

    // delete old fbo
    glDeleteBuffers(1, &m_fbo);
    glDeleteBuffers(1, &m_rbo);
    glDeleteTextures(1, &m_tex);

    // create a texture object
    glGenTextures(1, &m_tex);
    glBindTexture(GL_TEXTURE_2D, m_tex);
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_WindowWidth, m_WindowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);
    // create a renderbuffer object to store depth info
    glGenRenderbuffers(1, &m_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_WindowWidth, m_WindowHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // create a framebuffer object
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void vtkImguiAdapter::Render(void)
{

    if (m_fbo > 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        if (!m_IsInitiated) {
            m_vtkRenderWindow->InitializeFromCurrentContext();
            m_IsInitiated = true;
        }

        m_vtkRenderWindow->PushState();
        m_vtkRenderWindow->OpenGLInitState();
        glUseProgram(0);
        m_vtkRenderWindow->Render();
        m_vtkRenderWindow->PopState();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // these has to be called, otherwise imgui 
    // wouldn't show up
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // use the texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_tex);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, 0);
    glTexCoord2f(1.0, 0.0); glVertex3f(1.0, -1.0, 0);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, 1.0, 0);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

void vtkImguiAdapter::MouseButtonCallback(double xpos, double ypos, int button, int action, bool ctrl, bool shift, bool dclick)
{
    m_vtkRenderWindow->GetInteractor()->SetEventInformationFlipY(xpos, ypos, ctrl, shift, dclick);

    if (button == MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        m_vtkRenderWindow->GetInteractor()->InvokeEvent(vtkCommand::LeftButtonPressEvent, nullptr);
    if (button == MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        m_vtkRenderWindow->GetInteractor()->InvokeEvent(vtkCommand::LeftButtonReleaseEvent, nullptr);
    if (button == MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        m_vtkRenderWindow->GetInteractor()->InvokeEvent(vtkCommand::RightButtonPressEvent, nullptr);
    if (button == MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        m_vtkRenderWindow->GetInteractor()->InvokeEvent(vtkCommand::RightButtonReleaseEvent, nullptr);
}

void vtkImguiAdapter::MousePositionCallback(double xpos, double ypos, bool ctrl, bool shift)
{
    m_vtkRenderWindow->GetInteractor()->SetEventInformationFlipY(xpos, ypos, ctrl, shift, 0);
    m_vtkRenderWindow->GetInteractor()->InvokeEvent(vtkCommand::MouseMoveEvent, nullptr);
}

void vtkImguiAdapter::MouseWheelCallback(double xoffset, double yoffset)
{
    if (yoffset > 0)
        m_vtkRenderWindow->GetInteractor()->InvokeEvent(vtkCommand::MouseWheelForwardEvent, nullptr);
    else if (yoffset < 0)
        m_vtkRenderWindow->GetInteractor()->InvokeEvent(vtkCommand::MouseWheelBackwardEvent, nullptr);
}
