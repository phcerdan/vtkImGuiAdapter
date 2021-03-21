/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "vtkImGuiSDL2RenderWindowInteractor.h"
#include "vtkObjectFactory.h"
#include <SDL.h>

vtkStandardNewMacro(vtkImGuiSDL2RenderWindowInteractor);

//------------------------------------------------------------------------------
void vtkImGuiSDL2RenderWindowInteractor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "StartedMessageLoop: " << this->StartedMessageLoop << endl;
}

bool vtkImGuiSDL2RenderWindowInteractor::ProcessEvent(void* arg)
{
  if(!imguiIO) {
    throw std::runtime_error(" vtkImGuiSDL2RenderWindowInteractor: SetImguiIO before processing events");
  }

  SDL_Event* event = reinterpret_cast<SDL_Event*>(arg);
  SDL_Keymod modstates = SDL_GetModState();

  int alt = modstates & (KMOD_LALT | KMOD_RALT) ? 1 : 0;
  int shift = modstates & (KMOD_LSHIFT | KMOD_RSHIFT) ? 1 : 0;
  int ctrl = modstates & (KMOD_LCTRL | KMOD_RCTRL) ? 1 : 0;

  switch (event->type)
  {
    case SDL_QUIT:
      {
        return true;
      }
      break;

    case SDL_USEREVENT:
      {
        if (event->user.data1 == reinterpret_cast<void*>(vtkCommand::TimerEvent))
        {
          int tid = static_cast<int>(reinterpret_cast<int64_t>(event->user.data2));
          auto iter = this->VTKToPlatformTimerMap.find(tid);
          if (iter != this->VTKToPlatformTimerMap.end())
          {
            this->InvokeEvent(vtkCommand::TimerEvent, (void*)&tid);
            int ptid = (*iter).second;
            // Here we deal with one-shot versus repeating timers
            if (this->IsOneShotTimer(tid))
            {
              SDL_RemoveTimer(ptid);
            }
          }
        }
      }
      break;

    case SDL_KEYDOWN:
      {
        if(imguiIO->WantCaptureKeyboard) break;
        // simplified, not fully implemented
        std::string keyname = SDL_GetKeyName(event->key.keysym.sym);
        if (keyname.size())
        {
          this->SetKeyEventInformation(ctrl, shift, keyname[0], event->key.repeat, keyname.c_str());
          this->SetAltKey(alt);
          this->InvokeEvent(vtkCommand::KeyPressEvent, nullptr);
        }
      }
      break;

    case SDL_KEYUP:
      {
        if(imguiIO->WantCaptureKeyboard) break;
        // simplified, not fully implemented
        std::string keyname = SDL_GetKeyName(event->key.keysym.sym);
        if (keyname.size())
        {
          this->SetKeyEventInformation(ctrl, shift, keyname[0], event->key.repeat, keyname.c_str());
          this->SetAltKey(alt);
          this->InvokeEvent(vtkCommand::KeyReleaseEvent, nullptr);
          this->InvokeEvent(vtkCommand::CharEvent, nullptr);
        }
      }
      break;

    case SDL_MOUSEMOTION:
      {
        if(imguiIO->WantCaptureMouse) break;
        this->SetEventInformationFlipY(event->motion.x, event->motion.y, ctrl, shift);
        this->SetAltKey(alt);
        this->InvokeEvent(vtkCommand::MouseMoveEvent, nullptr);
      }
      break;

    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
      {
        if(imguiIO->WantCaptureMouse) break;
        this->SetEventInformationFlipY(event->button.x, event->button.y, ctrl, shift);
        this->SetAltKey(alt);

        int ev = -1;

        switch (event->button.button)
        {
          case SDL_BUTTON_LEFT:
            ev = event->button.state == SDL_PRESSED ? vtkCommand::LeftButtonPressEvent
              : vtkCommand::LeftButtonReleaseEvent;
            break;
          case SDL_BUTTON_MIDDLE:
            ev = event->button.state == SDL_PRESSED ? vtkCommand::MiddleButtonPressEvent
              : vtkCommand::MiddleButtonReleaseEvent;
            break;
          case SDL_BUTTON_RIGHT:
            ev = event->button.state == SDL_PRESSED ? vtkCommand::RightButtonPressEvent
              : vtkCommand::RightButtonReleaseEvent;
            break;
        }
        if (ev >= 0)
        {
          this->InvokeEvent(ev, nullptr);
        }
      }
      break;

    case SDL_MOUSEWHEEL:
      {
        if(imguiIO->WantCaptureMouse) break;
        this->SetControlKey(ctrl);
        this->SetShiftKey(shift);
        this->SetAltKey(alt);
        int ev = event->wheel.y > 0 ? vtkCommand::MouseWheelForwardEvent
          : vtkCommand::MouseWheelBackwardEvent;
        this->InvokeEvent(ev, nullptr);
      }
      break;

    case SDL_WINDOWEVENT:
      {
        switch (event->window.event)
        {
          case SDL_WINDOWEVENT_SIZE_CHANGED:
            {
              this->UpdateSize(event->window.data1, event->window.data2);
              this->Render(); // We are rendering at every frame
            }
            break;
          case SDL_WINDOWEVENT_CLOSE:
            {
              this->TerminateApp();
            }
            break;
        }
      }
      break;
  }
  return false;
}
