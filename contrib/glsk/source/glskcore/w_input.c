
/*
**
**		filename:		w_input.c
**
**		description:	input related functions
**		target-system:	microsoft windows
**		version-info:   $Id$
**
*/



#define INITGUID
#define CINTERFACE
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "w_main.h"
#include "w_input.h"
#include "w_window.h"

/*
	direct input system values
*/

static struct
{	
	// Direct Input Stuff
	LPDIRECTINPUT8					direct_input;
	int								enabled;

	struct
	{
		LPDIRECTINPUTDEVICE8		object;
		int							flags;
		char						buffer[ 256 ];
	}
	keyboard;

	struct
	{
		LPDIRECTINPUTDEVICE8		object;
		DIMOUSESTATE2				state;
		glsk_axial_device_t*		device;
	}
	mouse;

}
_glsk_input;

#define _GLSK_DEVICE_AQUIRED 1
#define _GLSK_DEVICE_HAS_MOUSEWHEEL 2

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

glsk_axial_device_t* glsk_get_mouse()
{
	return _glsk_input.mouse.device;
}

/*
	this updates the eactualy keyboard data in the buffer
*/

void _glsk_input_update()
{
	HRESULT result;

	if ( _glsk_input.enabled )
	{
		if ( _glsk_input.keyboard.flags & _GLSK_DEVICE_AQUIRED )
		{
			result = IDirectInputDevice8_GetDeviceState( _glsk_input.keyboard.object, 
				sizeof( _glsk_input.keyboard.buffer ), (LPVOID)&_glsk_input.keyboard.buffer ); 

			if ( FAILED( result ) )
			{
				return;
			}
		}

		if ( _glsk_input.mouse.device->flags & _GLSK_DEVICE_AQUIRED )
		{
			glsk_axial_device_t* device = _glsk_input.mouse.device;
			LPDIMOUSESTATE2 state = &_glsk_input.mouse.state;
			int i;

			result = IDirectInputDevice8_GetDeviceState( _glsk_input.mouse.object,
				sizeof( _glsk_input.mouse.state ), (LPVOID)state );

			if ( FAILED( result ) )
			{
				return;
			}

			// parse the data into the structure
			if ( device->flags & _GLSK_AXIAL_DEVICE_READ )
			{
				device->vector[ 0 ] = state->lX;
				device->vector[ 1 ] = -state->lY;
			}
			else
			{
				device->vector[ 0 ] += state->lX;
				device->vector[ 1 ] += -state->lY;
			}

			if ( device->flags & _GLSK_DEVICE_HAS_MOUSEWHEEL )
			{
				for ( i = 0; i < 3; ++i )
					device->button[ i ] = (state->rgbButtons[ i ] & 0x80) ? 1 : 0;

				for ( i = 3; i < device->button_count-2; ++i )
					device->button[ i+2 ] = (state->rgbButtons[ i ] & 0x80) ? 1 : 0;

				if ( state->lZ > 0 )
				{
					device->button[ 3 ] = 1;
					device->button[ 4 ] = 0;
				}
				else if ( state->lZ < 0 )
				{
					device->button[ 3 ] = 0;
					device->button[ 4 ] = 1;
				}
				
			}
			else
			{
				for ( i = 0; i < device->button_count; ++i )
					device->button[ i ] = state->rgbButtons[ i ] & 0x80;
			}

			//TODO: make sure there is really something new
			device->flags &= ~_GLSK_AXIAL_DEVICE_READ;
		}
	}

}

void _glsk_keyboard_clear_buffer()
{
	memset( _glsk_input.keyboard.buffer, 0, sizeof( _glsk_input.keyboard.buffer ) );
}

int glsk_axial_device_aquire( glsk_axial_device_t* device )
{
	if ( device == _glsk_input.mouse.device )
	{
		device->flags |= _GLSK_DEVICE_AQUIRED;
	}
	else
	{
	}

	if ( _glsk_input.enabled )
		_glsk_input_enable( 1 );

	return 1;
}

void glsk_axial_device_unaquire( glsk_axial_device_t* device )
{
	if ( device == _glsk_input.mouse.device )
	{
		device->flags  &= ~_GLSK_DEVICE_AQUIRED;
	}
	else
	{
	}
	
	if ( _glsk_input.enabled )
		_glsk_input_enable( 1 );
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
	return device->flags & _GLSK_DEVICE_AQUIRED;	
}

int glsk_axial_device_is_read( glsk_axial_device_t* device )
{
	return device->flags & _GLSK_AXIAL_DEVICE_READ;
}

void glsk_axial_device_mark_read( glsk_axial_device_t* device )
{
	device->flags |= _GLSK_AXIAL_DEVICE_READ;
}


/*
	this does fake aquiring - the real aquiring is done further down
*/

int glsk_keyboard_aquire()
{
	_glsk_input.keyboard.flags |= _GLSK_DEVICE_AQUIRED;
	
	if ( _glsk_input.enabled )
		_glsk_input_enable( 1 );

	return 1;
}

void glsk_keyboard_unaquire()
{
	_glsk_input.keyboard.flags &= ~_GLSK_DEVICE_AQUIRED;

	if ( _glsk_input.enabled )
		_glsk_input_enable( 1 );
}

int glsk_keyboard_is_aquired()
{
	return _glsk_input.keyboard.flags & _GLSK_DEVICE_AQUIRED;
}

/*
	aquire or unaquire all devices based on their flags
*/

void _glsk_input_enable( int value )
{
	// TODO: add an use flags

	_glsk_input.enabled = value;

	if ( value )
	{
		if ( _glsk_input.keyboard.flags & _GLSK_DEVICE_AQUIRED )
			IDirectInputDevice8_Acquire( _glsk_input.keyboard.object );
		else
		{
			IDirectInputDevice8_Unacquire( _glsk_input.keyboard.object );
			_glsk_keyboard_clear_buffer();
		}


		if ( _glsk_input.mouse.device->flags & _GLSK_DEVICE_AQUIRED )
			IDirectInputDevice8_Acquire( _glsk_input.mouse.object );
		else
			IDirectInputDevice8_Unacquire( _glsk_input.mouse.object );


	}
	else
	{
		if ( _glsk_input.keyboard.flags & _GLSK_DEVICE_AQUIRED )
		{
			IDirectInputDevice8_Unacquire( _glsk_input.keyboard.object );
			_glsk_keyboard_clear_buffer();
		}

		if ( _glsk_input.mouse.device->flags & _GLSK_DEVICE_AQUIRED )
		{
			IDirectInputDevice8_Unacquire( _glsk_input.mouse.object );
		}
	}
}

/*
	set the current owner window of the input
*/

int _glsk_input_set_owner( glsk_window_t* window )
{
	HRESULT result0, result1;

	// disable the input while switching
	_glsk_input_enable( 0 );

	// TODO: manage exclusive/non-exclusive mode somehow
	result0 = IDirectInputDevice8_SetCooperativeLevel( _glsk_input.keyboard.object, window->handle,
		DISCL_NONEXCLUSIVE | DISCL_FOREGROUND );

	result1 = IDirectInputDevice8_SetCooperativeLevel( _glsk_input.mouse.object, window->handle,
		DISCL_EXCLUSIVE | DISCL_FOREGROUND );

	_glsk_input_enable( 1 );

	if ( FAILED( result0 ) || FAILED( result1 ) )
		return 0;

	return 1;
}

/*
	startup the input system
*/

int _glsk_input_init()
{
	HRESULT result;

	// make sure the pointers have sane default values
	_glsk_input.direct_input = 0;
	_glsk_input.enabled = 0;

	_glsk_input.keyboard.object = 0;
	_glsk_input.keyboard.flags = 0;

	_glsk_input.mouse.object = 0;

	// create a DirectInput interface
	result = DirectInput8Create( global.hinstance, DIRECTINPUT_VERSION, &IID_IDirectInput8,
		(void**)&(_glsk_input.direct_input), NULL); 

	if ( FAILED( result ) )
	{
		_glsk_input_free();
		return 0;
	}

	// create a Keyboard device
	result = IDirectInput8_CreateDevice( _glsk_input.direct_input,
		&GUID_SysKeyboard, &_glsk_input.keyboard.object, NULL );
	
	if ( FAILED( result ) )
	{
		_glsk_input_free();
		return 0;
	}

	// create a mouse device
	result = IDirectInput8_CreateDevice( _glsk_input.direct_input,
		&GUID_SysMouse, &_glsk_input.mouse.object, NULL );

	if ( FAILED( result ) )
	{
		_glsk_input_free();
		return 0;
	}

	// set the keyboard data format
	result = IDirectInputDevice8_SetDataFormat( _glsk_input.keyboard.object, &c_dfDIKeyboard); 

	if ( FAILED( result ) )
	{
		_glsk_input_free();
		return 0;
	}

	// set the mouse data format
	result = IDirectInputDevice8_SetDataFormat( _glsk_input.mouse.object, &c_dfDIMouse2 );

	if ( FAILED( result ) )
	{
		_glsk_input_free();
		return 0;
	}
	else // get the mouse caps
	{
		DIDEVCAPS	caps;
		int			button_count = 0;
		int			flags = 0;

		caps.dwSize = sizeof( DIDEVCAPS );

		IDirectInputDevice8_GetCapabilities( _glsk_input.mouse.object, &caps );

		
		button_count = caps.dwButtons;

		// add two for the mousewheel
		if ( (caps.dwAxes >= 3) && (caps.dwButtons >= 3) )
		{
			flags |= _GLSK_DEVICE_HAS_MOUSEWHEEL;
			button_count += 2;
		}


		_glsk_input.mouse.device = _glsk_axial_device_new( 1, button_count );
		_glsk_input.mouse.device->flags = flags;
	}

	_glsk_keyboard_clear_buffer();

	return 1;
}

/*
	shutdown the input system
*/

void _glsk_input_free()
{
	if ( _glsk_input.keyboard.object )
		IDirectInputDevice8_Release( _glsk_input.keyboard.object );

	if ( _glsk_input.mouse.object )
		IDirectInputDevice8_Release( _glsk_input.mouse.object );

	if ( _glsk_input.direct_input )
		IDirectInput8_Release( _glsk_input.direct_input );

	_glsk_axial_device_free( _glsk_input.mouse.device );
}

int glsk_keyboard_get_keystate( int keycode )
{
	return _glsk_input.enabled && (_glsk_input.keyboard.buffer[ keycode ] & 0x80);
}
