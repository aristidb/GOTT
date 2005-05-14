#include "input.hpp"
#include "globals_x11.hpp"
#include <X11/Xdefs.h>
#include <X11/keysym.h>

namespace gott{namespace gui{

impl::key_table::key_table()
{
  std::memset( t, 0, 2*256 );

  t[ 1 ][ 'a' ] = t[ 1 ][ 'A' ] = KeyA;
  t[ 1 ][ 'b' ] = t[ 1 ][ 'B' ] = KeyB;
  t[ 1 ][ 'c' ] = t[ 1 ][ 'C' ] = KeyC;
  t[ 1 ][ 'd' ] = t[ 1 ][ 'D' ] = KeyD;
  t[ 1 ][ 'e' ] = t[ 1 ][ 'E' ] = KeyE;
  t[ 1 ][ 'f' ] = t[ 1 ][ 'F' ] = KeyF;
  t[ 1 ][ 'g' ] = t[ 1 ][ 'G' ] = KeyG;
  t[ 1 ][ 'h' ] = t[ 1 ][ 'H' ] = KeyH;
  t[ 1 ][ 'i' ] = t[ 1 ][ 'I' ] = KeyI;
  t[ 1 ][ 'j' ] = t[ 1 ][ 'J' ] = KeyJ;
  t[ 1 ][ 'k' ] = t[ 1 ][ 'K' ] = KeyK;
  t[ 1 ][ 'l' ] = t[ 1 ][ 'L' ] = KeyL;
  t[ 1 ][ 'm' ] = t[ 1 ][ 'M' ] = KeyM;
  t[ 1 ][ 'n' ] = t[ 1 ][ 'N' ] = KeyN;
  t[ 1 ][ 'o' ] = t[ 1 ][ 'O' ] = KeyO;
  t[ 1 ][ 'p' ] = t[ 1 ][ 'P' ] = KeyP;
  t[ 1 ][ 'q' ] = t[ 1 ][ 'Q' ] = KeyQ;
  t[ 1 ][ 'r' ] = t[ 1 ][ 'R' ] = KeyR;
  t[ 1 ][ 's' ] = t[ 1 ][ 'S' ] = KeyS;
  t[ 1 ][ 't' ] = t[ 1 ][ 'T' ] = KeyT;
  t[ 1 ][ 'u' ] = t[ 1 ][ 'U' ] = KeyU;
  t[ 1 ][ 'v' ] = t[ 1 ][ 'V' ] = KeyV;
  t[ 1 ][ 'w' ] = t[ 1 ][ 'W' ] = KeyW;
  t[ 1 ][ 'x' ] = t[ 1 ][ 'X' ] = KeyX;
  t[ 1 ][ 'y' ] = t[ 1 ][ 'Y' ] = KeyY;
  t[ 1 ][ 'z' ] = t[ 1 ][ 'Z' ] = KeyZ;

  t[ 1 ][ ' ' ] = KeySpace;
  t[ 1 ][ ';' ] = KeySemicolon;
  t[ 1 ][ ':' ] = KeyColon;
  t[ 1 ][ ',' ] = KeyComma;
  t[ 1 ][ '.' ] = KeyPeriod;
  t[ 1 ][ '/' ] = KeySlash;
  t[ 1 ][ '-' ] = KeyMinus;
  t[ 1 ][ '=' ] = KeyEquals;
  t[ 1 ][ '[' ] = KeyLBracket;
  t[ 1 ][ ']' ] = KeyRBracket;
  t[ 1 ][ '1' ] = Key1;
  t[ 1 ][ '2' ] = Key2;
  t[ 1 ][ '3' ] = Key3;
  t[ 1 ][ '4' ] = Key4;
  t[ 1 ][ '5' ] = Key5;
  t[ 1 ][ '6' ] = Key6;
  t[ 1 ][ '7' ] = Key7;
  t[ 1 ][ '8' ] = Key8;
  t[ 1 ][ '9' ] = Key9;
  t[ 1 ][ '0' ] = Key0;

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

key_code impl::key_table::translate_key( KeySym const& sym ) const
{
	if ( !sym )
		return NoKey;

	switch ( sym>>8 )
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
			return key_code( t[1][sym & 0xFF] );
		
		case 0xFF:
			return key_code( t[0][sym & 0xFF] );

		default:
			break;
	}
	
	return NoKey;
}


key_state::key_state()
  : keyboard( 512, 0)
{
}

}}
