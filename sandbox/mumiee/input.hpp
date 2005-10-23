// Copyright (C) 2004-2005 by Andreas Pokorny andreas.pokorny@gmail.com
// Content: GOTT Input Frontend Code
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


#ifndef GOTT_GUI_INPUT_HPP_INCLUDED
#define GOTT_GUI_INPUT_HPP_INCLUDED

#include <vector>
#include <gott/util/visibility.hpp>

#include "utility.hpp"

namespace gott{namespace gui{
  
struct GOTT_EXPORT mouse_state
{
  public:

  private:
    coord primary;
    coord secondary;

    char buttons[8];
    
    
  public:

    mouse_state();

    inline bool get_button( std::size_t index ) const { return buttons[index]==1; }
    inline int get_x_axis() const { return primary.x; }
    inline int get_y_axis() const { return primary.y; }
    inline int get_z_axis() const { return secondary.x; }
    inline coord const& get_primary_position() const { return primary; }
    inline coord const& get_secondary_position() const { return secondary; }

    inline void set_button( std::size_t index, bool state ) { buttons[index]=state; }
    inline void update_primary_position( coord const& movement ) { primary += movement; }
    inline void update_secondary_position( coord const& movement ) { secondary += movement; }
    inline void set_primary_position( coord const& p ) { primary = p; }
    inline void set_secondary_position( coord const& p ) { secondary = p; }


};

/**
 * Representing a pointer device event. 
 * handled through a union,
 */
struct GOTT_EXPORT mouse_event
{
  enum event_type
  { Move = 0, 
    Press, 
    Release 
  };
  event_type type;

  mouse_event( coord const& p, coord const& s )
  : type( Move ), primary(p), secondary(s) {}
  mouse_event( event_type const& type, std::size_t index, coord const& p, coord const& s)
    : type(type), primary(p), secondary(s), button_index(index) {}
  // add a time stamp here?
  coord primary;
  coord secondary;
  std::size_t button_index;
};

enum key_code {
  KeyEscape          = 0x00,
  Key1               = 0x02,
  Key2               = 0x03,
  Key3               = 0x04,
  Key4               = 0x05,
  Key5               = 0x06,
  Key6               = 0x07,
  Key7               = 0x08,
  Key8               = 0x09,
  Key9               = 0x0A,
  Key0               = 0x0B,
  KeyMinus           = 0x0C,    // - on main keyboard
  KeyEquals          = 0x0D,
  KeyBack            = 0x0E,    // backspace
  KeyTab             = 0x0F,
  KeyQ               = 0x10,
  KeyW               = 0x11,
  KeyE               = 0x12,
  KeyR               = 0x13,
  KeyT               = 0x14,
  KeyY               = 0x15,
  KeyU               = 0x16,
  KeyI               = 0x17,
  KeyO               = 0x18,
  KeyP               = 0x19,
  KeyLBracket        = 0x1A,
  KeyRBracket        = 0x1B,
  KeyReturn          = 0x1C,    // Enter on main keyboard
  KeyLControl        = 0x1D,
  KeyA               = 0x1E,
  KeyS               = 0x1F,
  KeyD               = 0x20,
  KeyF               = 0x21,
  KeyG               = 0x22,
  KeyH               = 0x23,
  KeyJ               = 0x24,
  KeyK               = 0x25,
  KeyL               = 0x26,
  KeySemicolon       = 0x27,
  KeyApostrophe      = 0x28,
  KeyGrave           = 0x29,    // accent grave
  KeyLShift          = 0x2A,
  KeyBackslash       = 0x2B,
  KeyZ               = 0x2C,
  KeyX               = 0x2D,
  KeyC               = 0x2E,
  KeyV               = 0x2F,
  KeyB               = 0x30,
  KeyN               = 0x31,
  KeyM               = 0x32,
  KeyComma           = 0x33,
  KeyPeriod          = 0x34,    // . on main keyboard
  KeySlash           = 0x35,    // / on main keyboard
  KeyRShift          = 0x36,
  KeyMultiply        = 0x37,    // * on numeric keypad
  KeyAlt             = 0x38,    // left Alt
  KeySpace           = 0x39,
  KeyCapital         = 0x3A,
  KeyF1              = 0x3B,
  KeyF2              = 0x3C,
  KeyF3              = 0x3D,
  KeyF4              = 0x3E,
  KeyF5              = 0x3F,
  KeyF6              = 0x40,
  KeyF7              = 0x41,
  KeyF8              = 0x42,
  KeyF9              = 0x43,
  KeyF10             = 0x44,
  KeyNumlock         = 0x45,
  KeyScroll          = 0x46,    // Scroll Lock
  KeyNumpad7         = 0x47,
  KeyNumpad8         = 0x48,
  KeyNumpad9         = 0x49,
  KeySubtract        = 0x4A,    // - on numeric keypad
  KeyNumpad4         = 0x4B,
  KeyNumpad5         = 0x4C,
  KeyNumpad6         = 0x4D,
  KeyAdd             = 0x4E,    // + on numeric keypad
  KeyNumpad1         = 0x4F,
  KeyNumpad2         = 0x50,
  KeyNumpad3         = 0x51,
  KeyNumpad0         = 0x52,
  KeyDecimal         = 0x53,    // . on numeric keypad 
  KeyOEM102          = 0x56,    // < > | on UK/Germany keyboards 
  KeyF11             = 0x57,
  KeyF12             = 0x58,
  KeyF13             = 0x64,    //                     (NEC PC98) 
  KeyF14             = 0x65,    //                     (NEC PC98) 
  KeyF15             = 0x66,    //                     (NEC PC98) 
  KeyKana            = 0x70,    // (Japanese keyboard)            
  KeyAbnt_c1         = 0x73,    // / ? on Portugese (Brazilian) keyboards 
  KeyConvert         = 0x79,    // (Japanese keyboard)            
  KeyNoConvert       = 0x7B,    // (Japanese keyboard)            
  KeyYen             = 0x7D,    // (Japanese keyboard)            
  KeyAbnt_c2         = 0x7E,    // Numpad . on Portugese (Brazilian) keyboards 
  KeyNumpadEquals    = 0x8D,    // = on numeric keypad (NEC PC98) 
  KeyPrevTrack       = 0x90,    // Previous Track (KeyCircumflex on Japanese keyboard) 
  KeyAt              = 0x91,    //                     (NEC PC98) 
  KeyColon           = 0x92,    //                     (NEC PC98) 
  KeyUnderline       = 0x93,    //                     (NEC PC98) 
  KeyKanji           = 0x94,    // (Japanese keyboard)            
  KeyStop            = 0x95,    //                     (NEC PC98) 
  KeyAx              = 0x96,    //                     (Japan AX) 
  KeyUnlabeled       = 0x97,    //                        (J3100) 
  KeyNextTrack       = 0x99,    // Next Track 
  KeyNumpadEnter     = 0x9C,    // Enter on numeric keypad 
  KeyRControl        = 0x9D,
  KeyMute            = 0xA0,    // Mute 
  KeyCalculator      = 0xA1,    // Calculator 
  KeyPlayPause       = 0xA2,    // Play / Pause 
  KeyMediaStop       = 0xA4,    // Media Stop 
  KeyVolumeDown      = 0xAE,    // Volume - 
  KeyVolumeUp        = 0xB0,    // Volume + 
  KeyWebHome         = 0xB2,    // Web home 
  KeyNumpadComma     = 0xB3,    // , on numeric keypad (NEC PC98) 
  KeyDivide          = 0xB5,    // / on numeric keypad 
  KeySysrq           = 0xB7,
  KeyAlt_gr          = 0xB8,    // right Alt 
  KeyPause           = 0xC5,    // Pause 
  KeyHome            = 0xC7,    // Home on arrow keypad 
  KeyUp              = 0xC8,    // UpArrow on arrow keypad 
  KeyPrior           = 0xC9,    // PgUp on arrow keypad 
  KeyLeft            = 0xCB,    // LeftArrow on arrow keypad 
  KeyRight           = 0xCD,    // RightArrow on arrow keypad 
  KeyEnd             = 0xCF,    // End on arrow keypad 
  KeyDown            = 0xD0,    // DownArrow on arrow keypad 
  KeyNext            = 0xD1,    // PgDn on arrow keypad 
  KeyInsert          = 0xD2,    // Insert on arrow keypad 
  KeyDelete          = 0xD3,    // Delete on arrow keypad 
  KeyLWin            = 0xDB,    // Left Windows key 
  KeyRWin            = 0xDC,    // Right Windows key 
  KeyApps            = 0xDD,    // AppMenu key 
  KeyPower           = 0xDE,    // System Power 
  KeySleep           = 0xDF,    // System Sleep 
  KeyWake            = 0xE3,    // System Wake 
  KeyWebSearch       = 0xE5,    // Web Search 
  KeyWebFavorites    = 0xE6,    // Web Favorites 
  KeyWebRefresh      = 0xE7,    // Web Refresh 
  KeyWebStop         = 0xE8,    // Web Stop 
  KeyWebForward      = 0xE9,    // Web Forward 
  KeyWebBack         = 0xEA,    // Web Back 
  KeyMyComputer      = 0xEB,    // My Computer 
  KeyMail            = 0xEC,    // Mail 
  KeyMediaSelect     = 0xED,    // Media Select 
  NoKey
};

struct GOTT_EXPORT key_event
{
  enum event_type{ Press, Release }; // Hold?
  event_type type;
  key_code code; 
  key_event( key_code code, event_type t ); 
};

struct GOTT_EXPORT key_state
{
  private:
    std::vector<unsigned char> keyboard;
  public:
    key_state();
    inline bool get_state( key_code code ) const {return keyboard[code]&1; }
    inline bool operator()( key_code code ) const {return keyboard[code]&1; }
    inline void set( key_code code ) {keyboard[code]=1; }
    inline void unset( key_code code ) {keyboard[code]=0; }
};

}}

#endif

