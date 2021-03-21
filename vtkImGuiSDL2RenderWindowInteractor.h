/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/**
 * @class   vtkImGuiSDL2RenderWindowInteractor
 * @brief   implements SDL2 specific functions to render with Imgui
 *
 */

#ifndef vtkImGuiSDL2RenderWindowInteractor_h
#define vtkImGuiSDL2RenderWindowInteractor_h

#include "vtkSDL2RenderWindowInteractor.h"

#include "vtkRenderingUIModule.h" // For export macro
#include "imgui.h"

class VTKRENDERINGUI_EXPORT vtkImGuiSDL2RenderWindowInteractor : public vtkSDL2RenderWindowInteractor
{
public:
  /** Inherit constructors from base class */
    using vtkSDL2RenderWindowInteractor::vtkSDL2RenderWindowInteractor;

  /**
   * Construct object so that light follows camera motion.
   */
  static vtkImGuiSDL2RenderWindowInteractor* New();

  vtkTypeMacro(vtkImGuiSDL2RenderWindowInteractor, vtkRenderWindowInteractor);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  inline void SetImguiIO(ImGuiIO * io) {imguiIO = io;};
  inline ImGuiIO * GetImguiIO() {return imguiIO;};

  /**
   * Same code than base class but public and not processing events when
   * ImGui wants to capture mouse or keyboard.
   */
  bool ProcessEvent(void* arg);
public:
  ImGuiIO * imguiIO;
};

#endif
