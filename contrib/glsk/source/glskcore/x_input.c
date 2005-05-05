
/*
**
**		filename:		x_input.c
**
**		description:	input related functions
**		target-system:	X-Windows
**		version-info:   $Id$
**
*/


// system includes
#include <stdlib.h>
#include <X11/X.h>
#include <X11/Xatom.h>
#include <GL/glx.h>
#include <X11/Xdefs.h>
#include <X11/keysym.h>
#include <X11/extensions/xf86vmode.h>

// global include
#include <glsk/glsk.h>

// internal includes
#include "x_main.h"
#include "x_input.h"

#define _GLSK_MOUSE_BUTTONS 8

/*
	the axial device structure
*/

struct glsk_axial_device_s
{
	int*		vector;
	int			vector_count;

	int*		button;
	int			button_count;

	int			flags;
};

static glsk_axial_device_t* _glsk_mouse = 0;

#define _GLSK_AXIAL_DEVICE_AQUIRED 1
#define _GLSK_AXIAL_DEVICE_READ 4

glsk_axial_device_t* _glsk_axial_device_new( int vector_count, int button_count )
{
	glsk_axial_device_t* result = (glsk_axial_device_t*)malloc( sizeof( glsk_axial_device_t ) );
	int i;

	if ( (result->vector_count = vector_count) > 0 )
	{
		result->vector = (int*)malloc( sizeof( int ) * vector_count * 2 );

		for ( i = 0; i < vector_count*2; ++i )
			result->vector[ i ] = 0;
	}
	else
	{
		result->vector = 0;
		result->vector_count = 0;
	}
	
	if ( (result->button_count = button_count) > 0 )
	{
		result->button = (int*)malloc( sizeof( int ) * button_count );

		for( i = 0; i < button_count; ++i )
			result->button[ i ] = 0;
	}
	else
	{
		result->button = 0;
		result->button_count = 0;
	}

	result->flags |= _GLSK_AXIAL_DEVICE_READ;

	return result;
}

void _glsk_axial_device_free( glsk_axial_device_t* device )
{
	if ( device == 0 )
		return;

	if ( device->vector )
		free( device->vector );

	if ( device->button )
		free( device->button );

	free( device );
}

#define _GLSK_KEYQUEUESIZE 64
#define _GLSK_KEYSTATESIZE 16 // 2 * 256 / 32

static int _glsk_keyqueue_size = 0;

static unsigned char _glsk_keytable[ 2 ][ 256 ];
static unsigned int _glsk_keystate[ _GLSK_KEYSTATESIZE ]; // each key uses 2 bits in here
static unsigned int _glsk_keyqueue[ _GLSK_KEYQUEUESIZE ];

#define _GLSK_KEYBOARD_AQUIRED 1

static int _glsk_input_flags = 0;

static int _glsk_input_init_keytable()
{
	int i;
	
	// init default
	for ( i=0; i < 256; ++i )
		_glsk_keytable[ 0 ][ i ] = _glsk_keytable[ 1 ][ i ] = 0;

	_glsk_keytable[ 1 ][ 'a' ] = _glsk_keytable[ 1 ][ 'A' ] = KEY_A;
	_glsk_keytable[ 1 ][ 'b' ] = _glsk_keytable[ 1 ][ 'B' ] = KEY_B;
	_glsk_keytable[ 1 ][ 'c' ] = _glsk_keytable[ 1 ][ 'C' ] = KEY_C;
	_glsk_keytable[ 1 ][ 'd' ] = _glsk_keytable[ 1 ][ 'D' ] = KEY_D;
	_glsk_keytable[ 1 ][ 'e' ] = _glsk_keytable[ 1 ][ 'E' ] = KEY_E;
	_glsk_keytable[ 1 ][ 'f' ] = _glsk_keytable[ 1 ][ 'F' ] = KEY_F;
	_glsk_keytable[ 1 ][ 'g' ] = _glsk_keytable[ 1 ][ 'G' ] = KEY_G;
	_glsk_keytable[ 1 ][ 'h' ] = _glsk_keytable[ 1 ][ 'H' ] = KEY_H;
	_glsk_keytable[ 1 ][ 'i' ] = _glsk_keytable[ 1 ][ 'I' ] = KEY_I;
	_glsk_keytable[ 1 ][ 'j' ] = _glsk_keytable[ 1 ][ 'J' ] = KEY_J;
	_glsk_keytable[ 1 ][ 'k' ] = _glsk_keytable[ 1 ][ 'K' ] = KEY_K;
	_glsk_keytable[ 1 ][ 'l' ] = _glsk_keytable[ 1 ][ 'L' ] = KEY_L;
	_glsk_keytable[ 1 ][ 'm' ] = _glsk_keytable[ 1 ][ 'M' ] = KEY_M;
	_glsk_keytable[ 1 ][ 'n' ] = _glsk_keytable[ 1 ][ 'N' ] = KEY_N;
	_glsk_keytable[ 1 ][ 'o' ] = _glsk_keytable[ 1 ][ 'O' ] = KEY_O;
	_glsk_keytable[ 1 ][ 'p' ] = _glsk_keytable[ 1 ][ 'P' ] = KEY_P;
	_glsk_keytable[ 1 ][ 'q' ] = _glsk_keytable[ 1 ][ 'Q' ] = KEY_Q;
	_glsk_keytable[ 1 ][ 'r' ] = _glsk_keytable[ 1 ][ 'R' ] = KEY_R;
	_glsk_keytable[ 1 ][ 's' ] = _glsk_keytable[ 1 ][ 'S' ] = KEY_S;
	_glsk_keytable[ 1 ][ 't' ] = _glsk_keytable[ 1 ][ 'T' ] = KEY_T;
	_glsk_keytable[ 1 ][ 'u' ] = _glsk_keytable[ 1 ][ 'U' ] = KEY_U;
	_glsk_keytable[ 1 ][ 'v' ] = _glsk_keytable[ 1 ][ 'V' ] = KEY_V;
	_glsk_keytable[ 1 ][ 'w' ] = _glsk_keytable[ 1 ][ 'W' ] = KEY_W;
	_glsk_keytable[ 1 ][ 'x' ] = _glsk_keytable[ 1 ][ 'X' ] = KEY_X;
	_glsk_keytable[ 1 ][ 'y' ] = _glsk_keytable[ 1 ][ 'Y' ] = KEY_Y;
	_glsk_keytable[ 1 ][ 'z' ] = _glsk_keytable[ 1 ][ 'Z' ] = KEY_Z;
	
	_glsk_keytable[ 1 ][ ' ' ] = KEY_SPACE;
	_glsk_keytable[ 1 ][ ';' ] = KEY_SEMICOLON;
	_glsk_keytable[ 1 ][ ':' ] = KEY_COLON;
	_glsk_keytable[ 1 ][ ',' ] = KEY_COMMA;
	_glsk_keytable[ 1 ][ '.' ] = KEY_PERIOD;
	_glsk_keytable[ 1 ][ '/' ] = KEY_SLASH;
	_glsk_keytable[ 1 ][ '-' ] = KEY_MINUS;
	_glsk_keytable[ 1 ][ '=' ] = KEY_EQUALS;
	_glsk_keytable[ 1 ][ '[' ] = KEY_LBRACKET;
	_glsk_keytable[ 1 ][ ']' ] = KEY_RBRACKET;
	_glsk_keytable[ 1 ][ '1' ] = KEY_1;
	_glsk_keytable[ 1 ][ '2' ] = KEY_2;
	_glsk_keytable[ 1 ][ '3' ] = KEY_3;
	_glsk_keytable[ 1 ][ '4' ] = KEY_4;
	_glsk_keytable[ 1 ][ '5' ] = KEY_5;
	_glsk_keytable[ 1 ][ '6' ] = KEY_6;
	_glsk_keytable[ 1 ][ '7' ] = KEY_7;
	_glsk_keytable[ 1 ][ '8' ] = KEY_8;
	_glsk_keytable[ 1 ][ '9' ] = KEY_9;
	_glsk_keytable[ 1 ][ '0' ] = KEY_0;

	_glsk_keytable[ 0 ][XK_BackSpace&0xFF] = KEY_BACK;
	_glsk_keytable[ 0 ][XK_Tab&0xFF] = KEY_TAB;
	_glsk_keytable[ 0 ][XK_Clear&0xFF] = KEY_SPACE;
	_glsk_keytable[ 0 ][XK_Return&0xFF] = KEY_RETURN;
	_glsk_keytable[ 0 ][XK_Pause&0xFF] = KEY_PAUSE;
	_glsk_keytable[ 0 ][XK_Escape&0xFF] = KEY_ESCAPE;
	_glsk_keytable[ 0 ][XK_Delete&0xFF] = KEY_DELETE;

	// the same keys w/ and w/o numlock - screw numlock
	_glsk_keytable[ 0 ][XK_KP_Insert&0xFF] = _glsk_keytable[ 0 ][XK_KP_0&0xFF] = KEY_NUMPAD0;
	_glsk_keytable[ 0 ][XK_KP_End&0xFF] = _glsk_keytable[ 0 ][XK_KP_1&0xFF] = KEY_NUMPAD1;
	_glsk_keytable[ 0 ][XK_KP_Down&0xFF] = _glsk_keytable[ 0 ][XK_KP_2&0xFF] = KEY_NUMPAD2;
	_glsk_keytable[ 0 ][XK_KP_Page_Down&0xFF] = _glsk_keytable[ 0 ][XK_KP_3&0xFF] =	KEY_NUMPAD3;
	_glsk_keytable[ 0 ][XK_KP_Left&0xFF] = _glsk_keytable[ 0 ][XK_KP_4&0xFF] = KEY_NUMPAD4;
	_glsk_keytable[ 0 ][XK_KP_Begin&0xFF] = _glsk_keytable[ 0 ][XK_KP_5&0xFF] = KEY_NUMPAD5;
	_glsk_keytable[ 0 ][XK_KP_Right&0xFF] = _glsk_keytable[ 0 ][XK_KP_6&0xFF] = KEY_NUMPAD6;
	_glsk_keytable[ 0 ][XK_KP_Home&0xFF] = _glsk_keytable[ 0 ][XK_KP_7&0xFF] = KEY_NUMPAD7;
	_glsk_keytable[ 0 ][XK_KP_Up&0xFF] = _glsk_keytable[ 0 ][XK_KP_8&0xFF] = KEY_NUMPAD8;
	_glsk_keytable[ 0 ][XK_KP_Page_Up&0xFF] = _glsk_keytable[ 0 ][XK_KP_9&0xFF] = KEY_NUMPAD9;

	_glsk_keytable[ 0 ][XK_KP_Delete&0xFF] = KEY_DELETE;
	_glsk_keytable[ 0 ][XK_KP_Decimal&0xFF] = KEY_PERIOD;
	_glsk_keytable[ 0 ][XK_KP_Divide&0xFF] = KEY_DIVIDE;
	_glsk_keytable[ 0 ][XK_KP_Multiply&0xFF] = KEY_MULTIPLY;
	_glsk_keytable[ 0 ][XK_KP_Subtract&0xFF] = KEY_MINUS;
	_glsk_keytable[ 0 ][XK_KP_Add&0xFF] = KEY_ADD;
	_glsk_keytable[ 0 ][XK_KP_Enter&0xFF] = KEY_NUMPADENTER;
	_glsk_keytable[ 0 ][XK_KP_Equal&0xFF] = KEY_NUMPADEQUALS;

	_glsk_keytable[ 0 ][XK_Up&0xFF] = KEY_UP;
	_glsk_keytable[ 0 ][XK_Down&0xFF] = KEY_DOWN;
	_glsk_keytable[ 0 ][XK_Right&0xFF] = KEY_RIGHT;
	_glsk_keytable[ 0 ][XK_Left&0xFF] = KEY_LEFT;
	_glsk_keytable[ 0 ][XK_Insert&0xFF] = KEY_INSERT;
	_glsk_keytable[ 0 ][XK_Home&0xFF] = KEY_HOME;
	_glsk_keytable[ 0 ][XK_End&0xFF] = KEY_END;
	_glsk_keytable[ 0 ][XK_Page_Up&0xFF] = KEY_PRIOR;
	_glsk_keytable[ 0 ][XK_Page_Down&0xFF] = KEY_NEXT;

	_glsk_keytable[ 0 ][XK_F1&0xFF] = KEY_F1;
	_glsk_keytable[ 0 ][XK_F2&0xFF] = KEY_F2;
	_glsk_keytable[ 0 ][XK_F3&0xFF] = KEY_F3;
	_glsk_keytable[ 0 ][XK_F4&0xFF] = KEY_F4;
	_glsk_keytable[ 0 ][XK_F5&0xFF] = KEY_F5;
	_glsk_keytable[ 0 ][XK_F6&0xFF] = KEY_F6;
	_glsk_keytable[ 0 ][XK_F7&0xFF] = KEY_F7;
	_glsk_keytable[ 0 ][XK_F8&0xFF] = KEY_F8;
	_glsk_keytable[ 0 ][XK_F9&0xFF] = KEY_F9;
	_glsk_keytable[ 0 ][XK_F10&0xFF] = KEY_F10;
	_glsk_keytable[ 0 ][XK_F11&0xFF] = KEY_F11;
	_glsk_keytable[ 0 ][XK_F12&0xFF] = KEY_F12;
	_glsk_keytable[ 0 ][XK_F13&0xFF] = KEY_F13;
	_glsk_keytable[ 0 ][XK_F14&0xFF] = KEY_F14;
	_glsk_keytable[ 0 ][XK_F15&0xFF] = KEY_F15;

	_glsk_keytable[ 0 ][XK_Num_Lock&0xFF] = KEY_NUMLOCK;
	_glsk_keytable[ 0 ][XK_Shift_R&0xFF] = KEY_RSHIFT;
	_glsk_keytable[ 0 ][XK_Shift_L&0xFF] = KEY_LSHIFT;
	_glsk_keytable[ 0 ][XK_Control_R&0xFF] = KEY_RCONTROL;
	_glsk_keytable[ 0 ][XK_Control_L&0xFF] = KEY_LCONTROL;
	_glsk_keytable[ 0 ][XK_Alt_R&0xFF] = KEY_ALT_GR;
	_glsk_keytable[ 0 ][XK_Alt_L&0xFF] = KEY_ALT;
	//_glsk_keytable[ 0 ][XK_Meta_R&0xFF] = KEY_UNKNOWN; // RMETA miis
	//_glsk_keytable[ 0 ][XK_Meta_L&0xFF] = KEY_UNKNOWN; // LMETA;
	_glsk_keytable[ 0 ][XK_Super_L&0xFF] = KEY_LWIN;
	_glsk_keytable[ 0 ][XK_Super_R&0xFF] = KEY_RWIN;
	_glsk_keytable[ 0 ][XK_Mode_switch&0xFF] = KEY_ALT_GR;
}

static int _glsk_input_translate_key( KeySym symbol )
{
	if ( !symbol )
		return 0;

	switch ( symbol>>8 )
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
			return _glsk_keytable[ 1 ][ symbol & 0xFF ];
		
		case 0xFF:
			return _glsk_keytable[ 0 ][ symbol & 0xFF ];

		default:
			break;
	}
	
	return 0;
}

static void _glsk_input_clear_keystate()
{
	int i;
	for ( i = 0; i < _GLSK_KEYSTATESIZE; ++i )
		_glsk_keystate[ i ] = 0;	
}

int _glsk_input_update()
{
	int i;
	// shift the statebuffer contents
	
	for ( i = 0; i < _GLSK_KEYSTATESIZE; ++i )
	{
		// copy the old state to the low an high bits
		_glsk_keystate[ i ] = (_glsk_keystate[ i ] & 0x55555555) |
							((_glsk_keystate[ i ] & 0x55555555) << 1);	
	}
	
	for ( i = 0; i < _glsk_keyqueue_size; ++i )
	{
		// update the new statebuffer
		// access the (n*2)th bit, which is the (n%16)*th bit in the ((n*2)/32)th=(n/16)th dword
		const int n = _glsk_keyqueue[ i ];
		const int keycode = n & 0xFF;
		
		if ( n >> 8 ) // the high bit set 
			_glsk_keystate[ keycode >> 4 ] |= 1 << ((keycode&15)<<1);
		else
			_glsk_keystate[ keycode >> 4 ] &= ~(1 << ((keycode&15)<<1));
	}
	
	_glsk_keyqueue_size = 0;	
}

void _glsk_input_key_event( KeySym symbol, int down )
{
	int keycode = _glsk_input_translate_key( symbol );
	
	if ( ((_glsk_input_flags&_GLSK_KEYBOARD_AQUIRED)==0) ||
		(_glsk_keyqueue_size >= _GLSK_KEYQUEUESIZE) || (!keycode) )
		return;
	
	//printf( "key %i is %s. (event: %i)\n", keycode, (down) ? "down" : "up",_glsk_keyqueue_size  );
	
	if ( down )
		_glsk_keyqueue[ _glsk_keyqueue_size++ ] = keycode | (1 << 8);
	else
		_glsk_keyqueue[ _glsk_keyqueue_size++ ] = keycode;		
}


int glsk_keyboard_aquire()
{
	_glsk_input_flags |= _GLSK_KEYBOARD_AQUIRED;
	
	return 1;
}

void glsk_keyboard_unaquire()
{
	_glsk_input_flags &= ~_GLSK_KEYBOARD_AQUIRED;
	
	_glsk_input_clear_keystate();
}

int glsk_keyboard_is_aquired()
{
	return _glsk_input_flags & _GLSK_KEYBOARD_AQUIRED;
}

int glsk_keyboard_get_keystate( int keycode )
{
	const int state = _glsk_keystate[ (keycode &= 0xFF) >> 4 ];
	const int bit = (keycode&15) << 1;
	
	//printf( "Checking key: %i with dword %i and bit %i\n", keycode, keycode >> 4, bit );
	
	return (state >> bit) & 3;	
}

int _glsk_input_init()
{
	_glsk_input_init_keytable();
	_glsk_input_clear_keystate();
	
	_glsk_mouse = _glsk_axial_device_new( 3, _GLSK_MOUSE_BUTTONS );
	
	return 1;
}

void _glsk_input_free()
{
	_glsk_axial_device_free( _glsk_mouse );
}

glsk_axial_device_t* glsk_get_mouse()
{
	return _glsk_mouse;
}

int _glsk_input_update_pointer_button( int index, int down )
{
	if ( _glsk_mouse->flags & _GLSK_AXIAL_DEVICE_AQUIRED )
	{
		if ( (0 < index) && (index <= _GLSK_MOUSE_BUTTONS) )
			_glsk_mouse->button[ index - 1 ] = down ? 1 : 0;
		
		_glsk_mouse->flags &= ~_GLSK_AXIAL_DEVICE_READ;
		
		return 1;
	}
	
	return 0;
}

int _glsk_input_update_pointer_move( int x, int y, glsk_window_t* window )
{
	if ( _glsk_mouse->flags & _GLSK_AXIAL_DEVICE_AQUIRED )
	{
		if ( window != 0 )
		{		
			int mx = window->width / 2;
			int my = window->height / 2;
		
			int dx = x-mx;
			int dy = y-my;
			
			//printf( "Mousemove!\n" );
		
			if ( dx || dy ) // first call
			{
				//printf( "(%i,%i) \n", dx, dy );
				if ( _glsk_mouse->flags & _GLSK_AXIAL_DEVICE_READ )
				{
					_glsk_mouse->vector[ 0 ] = mx;
					_glsk_mouse->vector[ 1 ] = -my;
				}
				else
				{
					_glsk_mouse->vector[ 0 ] += mx;
					_glsk_mouse->vector[ 1 ] += -my;
				}
				
				_glsk_mouse->flags &= ~_GLSK_AXIAL_DEVICE_READ;
				
				XWarpPointer( global.connection, None, window->handle,
								0, 0, 0, 0, mx, my );
			}
		}
	
		return 1;
	}
	
	return 0;
}

void _glsk_input_update_pointer_owner()
{
	glsk_window_t* wnd = global.focus_window;
	
	if ( _glsk_mouse->flags & _GLSK_AXIAL_DEVICE_AQUIRED )
	{
		if ( wnd )
		{		
			XGrabPointer( global.connection, wnd->handle, 0, PointerMotionMask|ButtonPressMask|ButtonReleaseMask,
				GrabModeAsync, GrabModeAsync, wnd->handle, global.blank_cursor, CurrentTime );
			
			XWarpPointer( global.connection, None, wnd->handle,
								0, 0, 0, 0, wnd->width/2, wnd->height/2 );
		}
		else
		{
			XUngrabPointer( global.connection, CurrentTime );
		}
	}
	else
	{
		XUngrabPointer( global.connection, CurrentTime );				
	}
}

int glsk_axial_device_aquire( glsk_axial_device_t* device )
{	
	device->flags |= _GLSK_AXIAL_DEVICE_AQUIRED;
	
	if ( device == _glsk_mouse )
		_glsk_input_update_pointer_owner();
	
	return 1;
}

void glsk_axial_device_unaquire( glsk_axial_device_t* device )
{
	int i = 0;
	
	for ( i = 0; i < device->button_count; ++i )
		device->button[ i ] = 0;
	
	device->flags &= ~_GLSK_AXIAL_DEVICE_AQUIRED;
		
	if ( device == _glsk_mouse )
		_glsk_input_update_pointer_owner();
}

int glsk_axial_device_get_button_count( glsk_axial_device_t* device )
{
	return device->button_count;
}

int glsk_axial_device_get_button_state( glsk_axial_device_t* device, int index )
{
	return device->button[ index ];
}

int glsk_axial_device_get_vector_count( glsk_axial_device_t* device )
{
	return device->vector_count;
}

void glsk_axial_device_get_vector_state( glsk_axial_device_t* device, int index, int* px, int* py )
{
	*px = device->vector[ (index<<1)   ];
	*py = device->vector[ (index<<1)|1 ];
}

int glsk_axial_device_is_aquired( glsk_axial_device_t* device )
{
	return device->flags & _GLSK_AXIAL_DEVICE_AQUIRED;	
}

int glsk_axial_device_is_read( glsk_axial_device_t* device )
{
	return device->flags & _GLSK_AXIAL_DEVICE_READ;
}

void glsk_axial_device_mark_read( glsk_axial_device_t* device )
{
	device->flags |= _GLSK_AXIAL_DEVICE_READ;
}
