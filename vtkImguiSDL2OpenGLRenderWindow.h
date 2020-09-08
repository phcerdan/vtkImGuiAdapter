/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/**
 * @class   vtkImguiSDL2OpenGLRenderWindow
 * @brief   implements SDL2 specific functions to render with Imgui
 *
 */

#ifndef vtkImguiSDL2OpenGLRenderWindow_h
#define vtkImguiSDL2OpenGLRenderWindow_h

#include "vtkSDL2OpenGLRenderWindow.h"

#include "vtkRenderingUIModule.h" // For export macro

class VTKRENDERINGUI_EXPORT vtkImguiSDL2OpenGLRenderWindow : public vtkSDL2OpenGLRenderWindow
{
public:
  /** Inherit constructors from base class */
    using vtkSDL2OpenGLRenderWindow::vtkSDL2OpenGLRenderWindow;

  /**
   * Construct object so that light follows camera motion.
   */
  static vtkImguiSDL2OpenGLRenderWindow* New();

  vtkTypeMacro(vtkImguiSDL2OpenGLRenderWindow, vtkSDL2OpenGLRenderWindow);

  /**
   * Override to avoid calling
   * SDL_GL_SwapWindow. The swap will be done in the app using Imgui.
   * A termination method performed at the end of the rendering process
   * to do things like swapping buffers (if necessary) or similar actions.
   */
  void Frame() override;
};

#endif
