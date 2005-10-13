// Copyright (C) 2004-2005 by Andreas Pokorny andreas.pokorny@gmail.com
// Content: GOTT Carbon Application class
// Authors: Andreas Pokorny
//
// This file is part of the Gott Project (http://gott.sf.net)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


#include "application.hpp"
#include <Carbon.h>

namespace gott{namespace gui{namespace carbon{
    
pascal OSStatus application_quit(EventHandlerCallRef nextHandler, EventRef theEvent, void* user_data);
pascal OSStatus application_mouse_down(EventHandlerCallRef nextHandler, EventRef theEvent, void* user_data);
pascal OSStatus application_mouse_up(EventHandlerCallRef nextHandler, EventRef theEvent, void* user_data);
pascal OSStatus application_mouse_dragged(EventHandlerCallRef nextHandler, EventRef theEvent, void* user_data);
pascal OSStatus application_key_down(EventHandlerCallRef nextHandler, EventRef theEvent, void* user_data);
pascal OSStatus application_key_up(EventHandlerCallRef nextHandler, EventRef theEvent, void* user_data);
pascal void application_idle(EventLoopTimerRef theTimer, void* user_data);

application::application(){

  // application
  EventTypeSpec		eventType;
  EventHandlerUPP		handlerUPP;

  eventType.eventClass = kEventClassApplication;
  eventType.eventKind = kEventAppQuit;

  handlerUPP = NewEventHandlerUPP( application_quit );

  InstallApplicationEventHandler( handlerUPP, 1, &eventType, nil, nil);

  eventType.eventClass = kEventClassMouse;
  eventType.eventKind = kEventMouseDown;
  handlerUPP = NewEventHandlerUPP( application_mouse_down );
  InstallApplicationEventHandler( handlerUPP, 1, &eventType, this, nil);

  eventType.eventKind = kEventMouseUp;
  handlerUPP = NewEventHandlerUPP( application_mouse_up );
  InstallApplicationEventHandler( handlerUPP, 1, &eventType, this, nil);

  eventType.eventKind = kEventMouseDragged;
  handlerUPP = NewEventHandlerUPP(application_mouse_dragged);
  InstallApplicationEventHandler( handlerUPP, 1, &eventType, this, nil);

  eventType.eventClass = kEventClassKeyboard;
  eventType.eventKind = kEventRawKeyDown;
  handlerUPP = NewEventHandlerUPP(application_key_down);
  InstallApplicationEventHandler( handlerUPP, 1, &eventType, this, nil);

  eventType.eventKind = kEventRawKeyUp;
  handlerUPP = NewEventHandlerUPP( application_key_up );
  InstallApplicationEventHandler( handlerUPP, 1, &eventType, this, nil);

  eventType.eventKind = kEventRawKeyRepeat;
  handlerUPP = NewEventHandlerUPP( application_key_down );		// 'key repeat' is translated to 'key down'
  InstallApplicationEventHandler( handlerUPP, 1, &eventType, this, nil);

  // Periodic task
  // Instead of an idle function I use the Carbon event timer.
  // You may decide to change the wait value which is currently 50 milliseconds.
/*  EventLoopRef		mainLoop;
  EventLoopTimerUPP	timerUPP;
  EventLoopTimerRef	theTimer;

  mainLoop = GetMainEventLoop();
  timerUPP = NewEventLoopTimerUPP( application_idle );
  InstallEventLoopTimer( mainLoop, 0, 50 * kEventDurationMillisecond, timerUPP, this, &theTimer);*/


}


application::run() 
{
  RunApplicationEventLoop();
}

pascal OSStatus application_quit (EventHandlerCallRef nextHandler, EventRef theEvent, void* user_data)
{
	user_data;
	
	return CallNextEventHandler (nextHandler, theEvent);
}


pascal OSStatus application_mouse_down (EventHandlerCallRef nextHandler, EventRef theEvent, void* user_data)
{
  Point wheresMyMouse;
  UInt32 modifier;

  GetEventParameter (theEvent, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(Point), NULL, &wheresMyMouse);
  GlobalToLocal (&wheresMyMouse);
  GetEventParameter (theEvent, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(UInt32), NULL, &modifier);

  platform_support * app = reinterpret_cast<platform_support*>(user_data);

  app->m_specific->m_cur_x = wheresMyMouse.h;
  if(app->flip_y())
  {
    app->m_specific->m_cur_y = app->rbuf_window().height() - wheresMyMouse.v;
  }
  else
  {
    app->m_specific->m_cur_y = wheresMyMouse.v;
  }
  app->m_specific->m_input_flags = mouse_left | get_key_flags(modifier);

  app->m_ctrls.set_cur(app->m_specific->m_cur_x, 
      app->m_specific->m_cur_y);
  if(app->m_ctrls.on_mouse_button_down(app->m_specific->m_cur_x, 
        app->m_specific->m_cur_y))
  {
    app->on_ctrl_change();
    app->force_redraw();
  }
  else
  {
    if(app->m_ctrls.in_rect(app->m_specific->m_cur_x, 
          app->m_specific->m_cur_y))
    {
      if(app->m_ctrls.set_cur(app->m_specific->m_cur_x, 
            app->m_specific->m_cur_y))
      {
        app->on_ctrl_change();
        app->force_redraw();
      }
    }
    else
    {
      app->on_mouse_button_down(app->m_specific->m_cur_x, 
          app->m_specific->m_cur_y, 
          app->m_specific->m_input_flags);
    }
  }

  return CallNextEventHandler (nextHandler, theEvent);
}


pascal OSStatus application_mouse_up (EventHandlerCallRef nextHandler, EventRef theEvent, void* user_data)
{
  Point wheresMyMouse;
  UInt32 modifier;

  GetEventParameter (theEvent, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(Point), NULL, &wheresMyMouse);
  GlobalToLocal (&wheresMyMouse);
  GetEventParameter (theEvent, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(UInt32), NULL, &modifier);

  platform_support * app = reinterpret_cast<platform_support*>(user_data);

  app->m_specific->m_cur_x = wheresMyMouse.h;
  if(app->flip_y())
  {
    app->m_specific->m_cur_y = app->rbuf_window().height() - wheresMyMouse.v;
  }
  else
  {
    app->m_specific->m_cur_y = wheresMyMouse.v;
  }
  app->m_specific->m_input_flags = mouse_left | get_key_flags(modifier);

  if(app->m_ctrls.on_mouse_button_up(app->m_specific->m_cur_x, 
        app->m_specific->m_cur_y))
  {
    app->on_ctrl_change();
    app->force_redraw();
  }
  app->on_mouse_button_up(app->m_specific->m_cur_x, 
      app->m_specific->m_cur_y, 
      app->m_specific->m_input_flags);

  return CallNextEventHandler (nextHandler, theEvent);
}


pascal OSStatus application_mouse_dragged (EventHandlerCallRef nextHandler, EventRef theEvent, void* user_data)
{
  Point wheresMyMouse;
  UInt32 modifier;

  GetEventParameter (theEvent, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(Point), NULL, &wheresMyMouse);
  GlobalToLocal (&wheresMyMouse);
  GetEventParameter (theEvent, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(UInt32), NULL, &modifier);

  platform_support * app = reinterpret_cast<platform_support*>(user_data);

  app->m_specific->m_cur_x = wheresMyMouse.h;
  if(app->flip_y())
  {
    app->m_specific->m_cur_y = app->rbuf_window().height() - wheresMyMouse.v;
  }
  else
  {
    app->m_specific->m_cur_y = wheresMyMouse.v;
  }
  app->m_specific->m_input_flags = mouse_left | get_key_flags(modifier);


  if(app->m_ctrls.on_mouse_move(
        app->m_specific->m_cur_x, 
        app->m_specific->m_cur_y,
        (app->m_specific->m_input_flags & mouse_left) != 0))
  {
    app->on_ctrl_change();
    app->force_redraw();
  }
  else
  {
    app->on_mouse_move(app->m_specific->m_cur_x, 
        app->m_specific->m_cur_y, 
        app->m_specific->m_input_flags);
  }

  return CallNextEventHandler (nextHandler, theEvent);
}


pascal OSStatus application_key_down (EventHandlerCallRef nextHandler, EventRef theEvent, void* user_data)
{
  char key_code;
  UInt32 modifier;

  GetEventParameter (theEvent, kEventParamKeyMacCharCodes, typeChar, NULL, sizeof(char), NULL, &key_code);
  GetEventParameter (theEvent, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(UInt32), NULL, &modifier);

  platform_support * app = reinterpret_cast<platform_support*>(user_data);

  app->m_specific->m_last_translated_key = 0;
  switch(modifier) 
  {
    case controlKey:
      app->m_specific->m_input_flags |= kbd_ctrl;
      break;

    case shiftKey:
      app->m_specific->m_input_flags |= kbd_shift;
      break;

    default:
      app->m_specific->translate(key_code);
      break;
  }

  if(app->m_specific->m_last_translated_key)
  {
    bool left  = false;
    bool up    = false;
    bool right = false;
    bool down  = false;

    switch(app->m_specific->m_last_translated_key)
    {
      case key_left:
        left = true;
        break;

      case key_up:
        up = true;
        break;

      case key_right:
        right = true;
        break;

      case key_down:
        down = true;
        break;

        //On a Mac, screenshots are handled by the system.
      case key_f2:                        
        app->copy_window_to_img(agg::platform_support::max_images - 1);
        app->save_img(agg::platform_support::max_images - 1, "screenshot");
        break;
    }


    if(app->m_ctrls.on_arrow_keys(left, right, down, up))
    {
      app->on_ctrl_change();
      app->force_redraw();
    }
    else
    {
      app->on_key(app->m_specific->m_cur_x,
          app->m_specific->m_cur_y,
          app->m_specific->m_last_translated_key,
          app->m_specific->m_input_flags);
    }
  }

  return CallNextEventHandler (nextHandler, theEvent);
}


pascal OSStatus application_key_up (EventHandlerCallRef nextHandler, EventRef theEvent, void* user_data)
{
  char key_code;
  UInt32 modifier;

  GetEventParameter (theEvent, kEventParamKeyMacCharCodes, typeChar, NULL, sizeof(char), NULL, &key_code);
  GetEventParameter (theEvent, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(UInt32), NULL, &modifier);

  platform_support * app = reinterpret_cast<platform_support*>(user_data);

  app->m_specific->m_last_translated_key = 0;
  switch(modifier) 
  {
    case controlKey:
      app->m_specific->m_input_flags &= ~kbd_ctrl;
      break;

    case shiftKey:
      app->m_specific->m_input_flags &= ~kbd_shift;
      break;
  }

  return CallNextEventHandler (nextHandler, theEvent);
}


pascal void application_idle( EventLoopTimerRef theTimer, void* user_data )
{
}


}






}}}
