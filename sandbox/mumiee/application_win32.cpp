
#include <windows.h>
#include <gl/gl.h>
#include <gl/glext.h>
#include "application.h"

namespace gott{namespace gui{
  
struct globals
{
  globals( HINSTANCE inst );
  ~globals();
  HINSTANCE	hinstance;
  TCHAR* appname;

  char*	extensions_buffer;
  char** extensions;
  int	extensions_count;

  DEVMODE previous_mode;

  int	flags;


#ifdef _DEBUG
#define DEBUG_BUFFER_SIZE 1024
  char debug_buffer[ DEBUG_BUFFER_SIZE ];
#endif

  struct time{
    hyper						temp;
    hyper						frequency;
    double						resolution;
    hyper						start;
  };

  bool init_timer();
  bool init_input();
  bool init_extensions();


  _glsk_wgl_extensions_t			wgl;

  glsk_opengl_interface_t			opengl_interface;
};

globals* global_data;

globals::globals( HINSTANCE inst )
  : hinstance(inst), flags(0) 
{
	HRESULT hresult = 0;

	appname = TEXT( "gott-gui-wnd-cls" );

	// save the old display setting
	EnumDisplaySettings( NULL, ENUM_CURRENT_SETTINGS, &previous_mode );

	// register the window class
	_glsk_register_class( hinstance, appname );

	// init direct input
	if ( init_input() == false )
		glsk_error_box( 0, "Unable to initialize subsystem: input" );

	// init opengl extensions
	if ( init_extensions() == false )
		glsk_error_box( 0, "Unable to initialize subsystem: opengl" );

	// init the timer
	if (  init_time() == false )
		glsk_error_box( 0, "Unable to initialize subsystem: timer" );
}


bool globals::init_timer()
{
  if( QueryPerformanceFrequency( (LARGE_INTEGER*)( &time.frequency ) ) )
	{
		// get start time
		QueryPerformanceCounter( (LARGE_INTEGER*)( &time.start ) );

		// Compute Resolution
		time.resolution	= 1.0 / time.frequency;

		return true;
	}	
	else
		return false;
}

bool globals::init_input()
{
}

bool globals::init_extensions()
{
}



globals::~globals()
{
	// shutdown the extensions subsystem
	_glsk_free_extensions();

	// shutdown the input subsystem
	_glsk_input_free();

	// TODO: shutdown and free all remaining windows

	if ( flags & GLSK_GLOBAL_DEVMODE_CHANGED ) // change it back
		ChangeDisplaySettings( &previous_mode, 0 );

	
	UnregisterClass( appname, hinstance );

	return 1;
}


application::status application::handle_pending_messages()
{
  MSG message;

	while ( PeekMessage( &message, NULL, 0, 0, PM_REMOVE ) )
	{
		if ( message.message == WM_QUIT )
			return end;

		TranslateMessage( &message );
		DispatchMessage( &message );	
	}
  return cont;
}


void application::quit()
{
	glsk_window_t* window = 0;

	while ( window )
	{
		glsk_window_destroy( window );
		window = window->next;
	}
  
	PostQuitMessage( 0 );
}


}}


#ifndef GOTT_GUI_STATIC

int WINAPI	DllMain( HINSTANCE hinstance, DWORD reason, PVOID reserved )
{
  try 
  {
    switch ( reason )
    {
      case DLL_PROCESS_ATTACH:
        gott::gui::global_data = new gott::gui::globals(hinstance);
        break;

      case DLL_PROCESS_DETACH:
        delete gott::gui::global_data;
        gott::gui::global_data = 0;
        break;
      default:
        return 1;
    }
  }
  catch(...)
  {
    return 0;
  }
  return 1;
}
#endif

int	main( int argc, char **argv );

namespace gott{namespace gui{namespace detail{
  
void parse_cmdline( char* cmdline, int* argc, char*** argv )
{
	int state = 0; // bit 1: last was valid, bit 2: in quotes
	int i = 0;

	*argc = 0;

	while ( cmdline[ i ] != '\0' )
	{
		if ( cmdline[ i ] == ' ' || cmdline[ i ] == '\t' )
		{
			if ( (state & 2) == 0 )
				state = 0;
		}
		else
		{
			if ( cmdline[ i ] == '\"' )
			{
				state ^= 2; // toggle the quote state
			}
			else
			{
				if ( (state & 1)==0 )
				{
					++(*argc);
					state |= 1;
				}
			}                                    
		}

		++i;
	}


	i = 0;
	state = 0;
	*argv = new char*[*argc];

  while ( *cmdline != '\0' )
	{
		if ( *cmdline == ' ' || *cmdline == '\t' )
		{
			if ( (state & 2) == 0 )
				state = 0;
			else
				*cmdline = '\0';
		}
		else
		{
			if ( *cmdline == '\"' )
			{
				state ^= 2; // toggle the quote state
				*cmdline = '\0';
			}
			else
			{
				if ( (state & 1)==0 )
				{
					(*argv)[ i ] = cmdline;
					++i;
					state |= 1;
				}
			}                                    
		}
		++cmdline;
	}
}

}}}

int WINAPI WinMain( HINSTANCE hinstance, HINSTANCE hlastinstance, PSTR cmdline, int iCmdShow )
{
	int			result = 0;
	int			argc = 0;
	char**		argv = 0;


  gott::gui::detail::parse_cmdline( GetCommandLine(), &argc, &argv );

#ifndef GOTT_GUI_STATIC

	result = main( argc, argv );

#else

  try {
    gott::gui::global_data = new gott::gui::globals(hinstance);
		result = main( argc, argv );
	}
	catch(...)
	{
	}
  delete gott::gui::global_data;


#endif // _glsk_compile_static_

	delete[] argv;

	return result;
}



