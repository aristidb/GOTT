// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is a user interface abstraction library.
 *
 * The Initial Developer of the Original Code is
 * Andreas Pokorny (andreas.pokorny@gmail.com)
 * Portions created by the Initial Developer are Copyright (C) 2005
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Andreas Pokorny (andreas.pokorny@gmail.com)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include <gott/ui/x11/input.hpp>

namespace gott { namespace ui { namespace x11 { namespace detail {

key_table const& key_table::get_instance() 
{
  static key_table instance;
  return instance;
}

key_table::key_table()
{
  std::memset(t, 0, 2*256);

  t[ 1 ][ int('a') ] = t[ 1 ][ int('A') ] = KeyA;
  t[ 1 ][ int('b') ] = t[ 1 ][ int('B') ] = KeyB;
  t[ 1 ][ int('c') ] = t[ 1 ][ int('C') ] = KeyC;
  t[ 1 ][ int('d') ] = t[ 1 ][ int('D') ] = KeyD;
  t[ 1 ][ int('e') ] = t[ 1 ][ int('E') ] = KeyE;
  t[ 1 ][ int('f') ] = t[ 1 ][ int('F') ] = KeyF;
  t[ 1 ][ int('g') ] = t[ 1 ][ int('G') ] = KeyG;
  t[ 1 ][ int('h') ] = t[ 1 ][ int('H') ] = KeyH;
  t[ 1 ][ int('i') ] = t[ 1 ][ int('I') ] = KeyI;
  t[ 1 ][ int('j') ] = t[ 1 ][ int('J') ] = KeyJ;
  t[ 1 ][ int('k') ] = t[ 1 ][ int('K') ] = KeyK;
  t[ 1 ][ int('l') ] = t[ 1 ][ int('L') ] = KeyL;
  t[ 1 ][ int('m') ] = t[ 1 ][ int('M') ] = KeyM;
  t[ 1 ][ int('n') ] = t[ 1 ][ int('N') ] = KeyN;
  t[ 1 ][ int('o') ] = t[ 1 ][ int('O') ] = KeyO;
  t[ 1 ][ int('p') ] = t[ 1 ][ int('P') ] = KeyP;
  t[ 1 ][ int('q') ] = t[ 1 ][ int('Q') ] = KeyQ;
  t[ 1 ][ int('r') ] = t[ 1 ][ int('R') ] = KeyR;
  t[ 1 ][ int('s') ] = t[ 1 ][ int('S') ] = KeyS;
  t[ 1 ][ int('t') ] = t[ 1 ][ int('T') ] = KeyT;
  t[ 1 ][ int('u') ] = t[ 1 ][ int('U') ] = KeyU;
  t[ 1 ][ int('v') ] = t[ 1 ][ int('V') ] = KeyV;
  t[ 1 ][ int('w') ] = t[ 1 ][ int('W') ] = KeyW;
  t[ 1 ][ int('x') ] = t[ 1 ][ int('X') ] = KeyX;
  t[ 1 ][ int('y') ] = t[ 1 ][ int('Y') ] = KeyY;
  t[ 1 ][ int('z') ] = t[ 1 ][ int('Z') ] = KeyZ;

  t[ 1 ][ int(' ') ] = KeySpace;
  t[ 1 ][ int(';') ] = KeySemicolon;
  t[ 1 ][ int(':') ] = KeyColon;
  t[ 1 ][ int(',') ] = KeyComma;
  t[ 1 ][ int('.') ] = KeyPeriod;
  t[ 1 ][ int('/') ] = KeySlash;
  t[ 1 ][ int('-') ] = KeyMinus;
  t[ 1 ][ int('=') ] = KeyEquals;
  t[ 1 ][ int('[') ] = KeyLBracket;
  t[ 1 ][ int(']') ] = KeyRBracket;
  t[ 1 ][ int('1') ] = Key1;
  t[ 1 ][ int('2') ] = Key2;
  t[ 1 ][ int('3') ] = Key3;
  t[ 1 ][ int('4') ] = Key4;
  t[ 1 ][ int('5') ] = Key5;
  t[ 1 ][ int('6') ] = Key6;
  t[ 1 ][ int('7') ] = Key7;
  t[ 1 ][ int('8') ] = Key8;
  t[ 1 ][ int('9') ] = Key9;
  t[ 1 ][ int('0') ] = Key0;

  t[ 0 ][XK_BackSpace&0xFF] = KeyBack;
  t[ 0 ][XK_Tab&0xFF] = KeyTab;
  t[ 0 ][XK_Clear&0xFF] = KeySpace;
  t[ 0 ][XK_Return&0xFF] = KeyReturn;
  t[ 0 ][XK_Pause&0xFF] = KeyPause;
  t[ 0 ][XK_Escape&0xFF] = KeyEscape;
  t[ 0 ][XK_Delete&0xFF] = KeyDelete;

  // the same keys w/ and w/o numlock - screw numlock
  t[ 0 ][XK_KP_Insert&0xFF] = t[ 0 ][XK_KP_0&0xFF] = KeyNumpad0;
  t[ 0 ][XK_KP_End&0xFF] = t[ 0 ][XK_KP_1&0xFF] = KeyNumpad1;
  t[ 0 ][XK_KP_Down&0xFF] = t[ 0 ][XK_KP_2&0xFF] = KeyNumpad2;
  t[ 0 ][XK_KP_Page_Down&0xFF] = t[ 0 ][XK_KP_3&0xFF] =	KeyNumpad3;
  t[ 0 ][XK_KP_Left&0xFF] = t[ 0 ][XK_KP_4&0xFF] = KeyNumpad4;
  t[ 0 ][XK_KP_Begin&0xFF] = t[ 0 ][XK_KP_5&0xFF] = KeyNumpad5;
  t[ 0 ][XK_KP_Right&0xFF] = t[ 0 ][XK_KP_6&0xFF] = KeyNumpad6;
  t[ 0 ][XK_KP_Home&0xFF] = t[ 0 ][XK_KP_7&0xFF] = KeyNumpad7;
  t[ 0 ][XK_KP_Up&0xFF] = t[ 0 ][XK_KP_8&0xFF] = KeyNumpad8;
  t[ 0 ][XK_KP_Page_Up&0xFF] = t[ 0 ][XK_KP_9&0xFF] = KeyNumpad9;

  t[ 0 ][XK_KP_Delete&0xFF] = KeyDelete;
  t[ 0 ][XK_KP_Decimal&0xFF] = KeyPeriod;
  t[ 0 ][XK_KP_Divide&0xFF] = KeyDivide;
  t[ 0 ][XK_KP_Multiply&0xFF] = KeyMultiply;
  t[ 0 ][XK_KP_Subtract&0xFF] = KeyMinus;
  t[ 0 ][XK_KP_Add&0xFF] = KeyAdd;
  t[ 0 ][XK_KP_Enter&0xFF] = KeyNumpadEnter;
  t[ 0 ][XK_KP_Equal&0xFF] = KeyNumpadEquals;

  t[ 0 ][XK_Up&0xFF] = KeyUp;
  t[ 0 ][XK_Down&0xFF] = KeyDown;
  t[ 0 ][XK_Right&0xFF] = KeyRight;
  t[ 0 ][XK_Left&0xFF] = KeyLeft;
  t[ 0 ][XK_Insert&0xFF] = KeyInsert;
  t[ 0 ][XK_Home&0xFF] = KeyHome;
  t[ 0 ][XK_End&0xFF] = KeyEnd;
  t[ 0 ][XK_Page_Up&0xFF] = KeyPrior;
  t[ 0 ][XK_Page_Down&0xFF] = KeyNext;

  t[ 0 ][XK_F1&0xFF] = KeyF1;
  t[ 0 ][XK_F2&0xFF] = KeyF2;
  t[ 0 ][XK_F3&0xFF] = KeyF3;
  t[ 0 ][XK_F4&0xFF] = KeyF4;
  t[ 0 ][XK_F5&0xFF] = KeyF5;
  t[ 0 ][XK_F6&0xFF] = KeyF6;
  t[ 0 ][XK_F7&0xFF] = KeyF7;
  t[ 0 ][XK_F8&0xFF] = KeyF8;
  t[ 0 ][XK_F9&0xFF] = KeyF9;
  t[ 0 ][XK_F10&0xFF] = KeyF10;
  t[ 0 ][XK_F11&0xFF] = KeyF11;
  t[ 0 ][XK_F12&0xFF] = KeyF12;
  t[ 0 ][XK_F13&0xFF] = KeyF13;
  t[ 0 ][XK_F14&0xFF] = KeyF14;
  t[ 0 ][XK_F15&0xFF] = KeyF15;

  t[ 0 ][XK_Num_Lock&0xFF] = KeyNumlock;
  t[ 0 ][XK_Shift_R&0xFF] = KeyRShift;
  t[ 0 ][XK_Shift_L&0xFF] = KeyLShift;
  t[ 0 ][XK_Control_R&0xFF] = KeyRControl;
  t[ 0 ][XK_Control_L&0xFF] = KeyLControl;
  t[ 0 ][XK_Alt_R&0xFF] = KeyAlt_gr;
  t[ 0 ][XK_Alt_L&0xFF] = KeyAlt;
  //t[ 0 ][XK_Meta_R&0xFF] = KeyUnknown; // RMETA miis
  //t[ 0 ][XK_Meta_L&0xFF] = KeyUnknown; // LMETA;
  t[ 0 ][XK_Super_L&0xFF] = KeyLWin;
  t[ 0 ][XK_Super_R&0xFF] = KeyRWin;
  t[ 0 ][XK_Mode_switch&0xFF] = KeyAlt_gr;
}

key_code key_table::translate_key(KeySym const& sym) const
{
        if (!sym)
        	return NoKey;

        switch (sym>>8)
        {
        	case 0x00:	// Latin 1
        	case 0x01:	// Latin 2
        	case 0x02:	// Latin 3
        	case 0x03:	// Latin 4
        	case 0x04:	// Katakana
        	case 0x05:	// Arabic
        	case 0x06:	// Cyrillic
        	case 0x07:	// Greek
        	case 0x08:	// Technical
        	case 0x0A:	// Publishing
        	case 0x0C:	// Hebrew
        	case 0x0D:	// Thai
        		return key_code(t[1][sym & 0xFF]);
        	
        	case 0xFF:
        		return key_code(t[0][sym & 0xFF]);

        	default:
        		break;
        }
        
        return NoKey;
}

}}}}

