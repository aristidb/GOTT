

#include <iostream>
#include <stdexcept>
#include <X11/X.h>
#include <X11/Xatom.h>
#include <GL/glx.h>
#include <boost/cstdint.hpp>
#include <X11/extensions/xf86vmode.h>
#include "window.hpp"
#include "globals_x11.hpp"
#include "application.hpp"

namespace gott{ namespace gui{

window::os_specific::os_specific()
  : handle(None), drawable(None)
{
}

#define set_pf_value( a, b, v, c ) { (v)[ (c)++ ] = (a); (v)[ (c)++ ] = (b); }

XVisualInfo* get_visualinfo( pixelformat const& format )
{
	int						values[ 24 ];
	int						c = 0;
	
	values[ c++ ] = GLX_RGBA;
	
	if( format.flags & pixelformat::DoubleBuffer )
		values[ c++ ] = GLX_DOUBLEBUFFER;
		
	if( format.color.first )
	{
    switch( format.color.second )
    {
      case 32:
      case 24:
        set_pf_value( GLX_RED_SIZE, 8 , values, c);
        set_pf_value( GLX_GREEN_SIZE, 8, values, c );
        set_pf_value( GLX_BLUE_SIZE, 8, values, c );
        break;

      case 16:
        set_pf_value( GLX_RED_SIZE, 5, values, c );
        set_pf_value( GLX_GREEN_SIZE, 6, values, c );
        set_pf_value( GLX_BLUE_SIZE, 5, values, c );
        break;
      default:
        return 0;
    }
  }
	
	values[ c++ ] = None;
	
	return glXChooseVisual( global_data.connection, global_data.screen, values );
}

GLXFBConfig	get_fbconfig( pixelformat const& format )
{
	GLXFBConfig*			Array = 0;
	GLXFBConfig				Result = 0;
	int						values[ 24 ];
	int						c = 0;
	int						bits = 0;
	int						elements = 0;

	if( format.flags & pixelformat::DrawToWindow )
		bits |= GLX_WINDOW_BIT_SGIX;

	if(format.flags & pixelformat::DrawToPBuffer)
		bits |= GLX_PBUFFER_BIT_SGIX;

	set_pf_value( GLX_DRAWABLE_TYPE_SGIX, bits, values, c );

	if( format.flags & pixelformat::DoubleBuffer )
		set_pf_value( GLX_DOUBLEBUFFER, 1, values, c );

	if( format.color.first )
	{
    switch( format.color.second )
    {
      case 32:
      case 24:
        set_pf_value( GLX_RED_SIZE, 8 , values, c);
        set_pf_value( GLX_GREEN_SIZE, 8, values, c );
        set_pf_value( GLX_BLUE_SIZE, 8, values, c );
        break;

      case 16:
        set_pf_value( GLX_RED_SIZE, 5, values, c );
        set_pf_value( GLX_GREEN_SIZE, 6, values, c );
        set_pf_value( GLX_BLUE_SIZE, 5, values, c );
        break;
      default:
        return 0;
    }
  }

	if( format.depth.first )
	{
		set_pf_value( GLX_DEPTH_SIZE,format.depth.second, values, c );
	}

	if( format.stencil.first )
	{
		set_pf_value( GLX_STENCIL_SIZE, format.depth.second, values, c );
	}

	if( format.samples.first ) 
	{
		if( global_data.is_extension_supported( "GL_ARB_multisample" ) )
		{
			set_pf_value( GLX_SAMPLE_BUFFERS_ARB, 1, values, c );
	
			if( format.samples.second )
				set_pf_value( GLX_SAMPLES_ARB, format.samples.second, values, c );
		}
		else
		{
		}
	}

	values[ c++ ] = None;


	Array = glXChooseFBConfig( global_data.connection, global_data.screen, values, &elements );

	if ( elements )
	{
		Result = Array[ 0 ];
		XFree( Array );
		
		return Result;
	}

	return None;
}

#undef set_pf_value

window::window( gl_context const& c, pixelformat const& format, std::size_t fl )
  : os(new os_specific)
  , context(c)
  , flags(Clear)
{
	XSetWindowAttributes	attributes;
	unsigned int attributes_mask = 0;
	XVisualInfo* visual_info = 0;
	GLXFBConfig fb_config;
	Window root_window = RootWindow( global_data.connection, global_data.screen );
	Atom wm_delete_atom = XInternAtom( global_data.connection, "WM_DELETE_WINDOW", true );

  std::cout << " glx:version " << global_data.glx_version[ 0 ] << " . " << global_data.glx_version[ 1 ] << std::endl;
	if( global_data.glx_version[ 0 ] < 1 )
		throw std::runtime_error("glx version");

 	// init the pixelformat and get the associated visual info

	// run the user-handler

	if( global_data.glx_fallback_mode == 0 )
	{
		// select the appropriate fbconfig
		fb_config = get_fbconfig( format );
		
		// did we find a suitable fb_config?
		if ( fb_config == None )
      throw std::runtime_error("no suitable fb config");
		
		visual_info = glXGetVisualFromFBConfig( global_data.connection,	fb_config );
	}
	else
	{
		visual_info = get_visualinfo( format );
	}
		
	// did we find a matching visual info too?
	if ( visual_info == 0 )
		throw std::runtime_error( "unable to find requested visual." );
	
	// init the colormap
	attributes.colormap = XCreateColormap( global_data.connection, root_window,
					visual_info->visual, AllocNone );

	attributes.border_pixel = 0;

	attributes.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
		StructureNotifyMask | PointerMotionMask | FocusChangeMask;

	attributes.override_redirect = true;

	attributes_mask = CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect;

	// create the physical window
	os->handle = XCreateWindow( global_data.connection, root_window
      , 100
      , 100
      , 100  //FIXME!
      , 100 
      , 0, visual_info->depth, InputOutput, visual_info->visual, 
		attributes_mask, &attributes );

	if( os->handle == None )
    throw std::runtime_error("no window handle");
		if( fl & Decoration )
    set_decoration();

  set_title( "no title" );
	XSync( global_data.connection, 0 );
	// _glsk_wait_for_map_notify( window->handle );
	
	//set_position(  );
	
	// flag this window as open
	flags |= window::Open;
	
	XSetWMProtocols( global_data.connection, os->handle, &wm_delete_atom, 1 );

	// set the object pointer -TODO: make this 64-bit compatible
	//printf( "Created window with address: %p\n", window );
/*	XChangeProperty( global_data.connection, os->handle, global_data.object_atom, global_data.object_atom,
		32, PropModeReplace, reinterpret_cast<unsigned char*>(this), sizeof( window* )/sizeof(boost::int32_t) );*/

	// fallback if we are below GLX version 1.3 (damn ATI drivers)
	if( global_data.glx_fallback_mode == 0 ) // GLX >= 1.3
	{
		if( context.os->handle == None )
		{
			// create the GL context
			context.os->handle = glXCreateNewContext( global_data.connection, fb_config, GLX_RGBA_TYPE, 0, true);
		
			if( context.os->handle == None )
        throw std::runtime_error("glXCreateNewContext did not yield a context");
			
			if( !glXIsDirect( global_data.connection, context.os->handle ) )
        throw std::runtime_error("glX is not direct - screw you!" );
		}
		
		os->drawable = glXCreateWindow( global_data.connection, fb_config, os->handle, 0 );
		
		if( os->drawable == None )
      throw std::runtime_error("no drawable" );
	}
	else // GLX <= 1.2
	{
		
		printf( "(glsk) warning: using glx version < 1.3 fallback\n" );
		
		if( context.os->handle == None )
		{
			context.os->handle = glXCreateContext( global_data.connection, visual_info, 0, true );
			
			if ( context.os->handle == None )
        throw std::runtime_error("glXCreateContext did not yield a context");
	
			if ( !glXIsDirect( global_data.connection, context.os->handle ) )
        throw std::runtime_error("glX is not direct - screw you!" );
		}
	}
	
  // Get the current attributes .. lets hope the window manager already reset the window sizes:
  XWindowAttributes attr;
  XGetWindowAttributes( global_data.connection, os->handle, & attr );

  px = attr.x;
  py = attr.y;
  width = attr.width;
  height = attr.height;

  
  application::get_instance().add_window( this );
  
  create( width, height );

  if( fl & Visible )
    set_visible();
}

void window::set_title(std::string const & t)
{
  title = t;

  if( os->handle )
  {
    // convert the string we got to a text property
    XTextProperty property;
    char *a = const_cast<char*>(title.c_str());

    if( XmbTextListToTextProperty( global_data.connection, &a, 1, XTextStyle , &property ) != Success )
      return;

    // set the physical window name
    XSetWMName( global_data.connection, os->handle, &property );

    XFree( property.value );
  }
}

void window::set_visible( bool vis )
{
  if( os->handle )
    if( vis )
    {
      flags |= Visible;
      XMapWindow( global_data.connection, os->handle );
    }
    else 
    {
      flags &= ~Visible;
      XUnmapWindow( global_data.connection, os->handle );
    }
}



void window::set_decoration( bool st )
{
  flags |= st * Decoration;
 os->wm_type = XInternAtom( global_data.connection, "_NET_WM_WINDOW_TYPE", true );
  Atom hint = XInternAtom(global_data.connection,"_NET_WM_WINDOW_TYPE_NORMAL", true );
  XChangeProperty(global_data.connection, os->handle,os->wm_type, XA_ATOM, 32, PropModeReplace,reinterpret_cast<unsigned char*>(&hint),1);
  
/*  if( os->handle )
  {
    boost::tuple<unsigned long, unsigned long
    glsk_motif_wm_hints_t   hints_object;
    glsk_motif_wm_hints_t*  hints_pointer  = 0;
    Atom          hints_atom = None;
    int           return_format = 0;
    Atom          return_type = None;
    unsigned long     return_count = 0;
    unsigned long     return_rest = 0;
    XSetWindowAttributes  attributes;

    attributes.override_redirect = FALSE;

    XChangeWindowAttributes( global_data.connection, os->handle, CWOverrideRedirect, &attributes );

    if( st )
    {

      //XMapRaised( global.connection, window->handle );
      XUnmapWindow( global_data.connection, os->handle );
      XMapWindow( global_data.connection, os->handle );
    }
    XFlush( global_data.connection );


    //printf( "changing decoration %i\n", has_decoration );

    hints_object.flags = MWM_HINTS_DECORATIONS;
    hints_object.decorations = st;

    // try getting the window-decoration(hints) extension atom    
    hints_atom = XInternAtom( global_data.connection, _XA_MOTIF_WM_HINTS, FALSE );
    XGetWindowProperty ( global_data.connection, os->handle,
        hints_atom, 0, sizeof(glsk_motif_wm_hints_t)/sizeof(int),
        FALSE, AnyPropertyType, &return_type, &return_format, &return_count,
        &return_rest, (unsigned char **)&hints_pointer);

    if ( return_type != None )
    {
      hints_pointer->flags |= MWM_HINTS_DECORATIONS;
      hints_pointer->decorations = hints_object.decorations;
    }
    else
    {
      hints_pointer = &hints_object;
    }

    XChangeProperty( global_data.connection, window->handle, hints_atom,
        hints_atom, 32, PropModeReplace, (unsigned char *)hints_pointer,
        sizeof (glsk_motif_wm_hints_t)/sizeof (int));

    if( hints_pointer != &hints_object )
      XFree( hints_pointer );

    XFlush( global_data.connection );
    //;
    }*/
}

void window::set_position( int x, int y)
{
  if( px != x || py != y )
  {
    px = x;
    py = y;

    if( os->handle )
    {
      Window root_window = RootWindow( global_data.connection, global_data.screen );
      XWindowAttributes root_attribs;
      XGetWindowAttributes( global_data.connection, root_window, &root_attribs );

      y = root_attribs.height - height - y;

      XMoveWindow( global_data.connection, os->handle, x, y );   

      // now get the current position:
      XWindowAttributes attr;
      XGetWindowAttributes( global_data.connection, os->handle, &attr );

      px = attr.x;
      py = attr.y;
      // debug that
    }
  }
}

void window::set_size( std::size_t w, std::size_t h )
{
  if( os->handle )
  {
    XWindowChanges changes;
    Window root_window = RootWindow( global_data.connection, global_data.screen );
    XWindowAttributes root_attribs;
    
    XGetWindowAttributes( global_data.connection, root_window, &root_attribs );

    changes.x = px;
    changes.y = root_attribs.height - height - py;
    changes.width  = width = w;
    changes.height = height = h;

    XConfigureWindow( global_data.connection, os->handle,
          CWWidth | CWHeight | CWX | CWY, &changes );
  }
}


window::~window()
{
  application::get_instance().remove_window( this );
  delete os;
}

void globals::process_event( window * win, XEvent const& event )
{
  switch( event.type )
  {
    case ReparentNotify:
      {
        std::cout << "ReparentNotify" << std::endl;
        if( (win->flags & window::Decoration) == 0 )
        {
          XSetWindowAttributes  attributes;

          // apparently, we just released our decoration - turn on the redirect override_redirect
          attributes.override_redirect = true;

          XChangeWindowAttributes( connection, win->os->handle, CWOverrideRedirect, &attributes );
        }

        break;
      };
      

    case MotionNotify:
      {
        std::cout << "MotionNotify" << std::endl;
        /*if ( _glsk_input_update_pointer_move( event.xmotion.x, event.xmotion.y, win) == 0 )
          _glsk_send_mouse_event( window, event.xmotion.x, window->height-event.xmotion.y,
          GLSK_ME_TYPE_MOVE, 0 );*/
        break;
      };

    case ButtonPress:
      {
        std::cout << "ButtonPress" << std::endl;
        XSetInputFocus( global_data.connection, win->os->handle, RevertToPointerRoot, CurrentTime );

        /*if ( _glsk_input_update_pointer_button( event.xbutton.button, 1 ) == 0 )
          _glsk_send_mouse_event( win, event.xbutton.x, win->height-event.xbutton.y,
              GLSK_ME_TYPE_BUTTONPRESSED, event.xbutton.button );*/

        break;
      };

    case ButtonRelease:
      {
        std::cout << "ButtonRelease" << std::endl;
        /*if ( _glsk_input_update_pointer_button( event.xbutton.button, 0 ) == 0 )
          _glsk_send_mouse_event( win, event.xbutton.x, win->height-event.xbutton.y,
              GLSK_ME_TYPE_BUTTONRELEASED, event.xbutton.button );
*/
        break;
      };

    case MapNotify:
      {
        std::cout << "MapNotify" << std::endl;
        XSetInputFocus( global_data.connection, win->os->handle, RevertToPointerRoot, CurrentTime );
        break;
      };

    case DestroyNotify:
      {
        std::cout << "DestroyNotify" << std::endl;
        win->os->handle = None;
        break;
      };

    case KeyPress:
      {
        std::cout << "KeyPress" << std::endl;
        XKeyEvent sym = event.xkey;
        key_code c = keys.translate_key( XLookupKeysym( &sym, 0) );

        application::get_instance().get_key_state().set(c);
        if ( win && win->flags & window::KeyEvents )
          win->key( key_event( c, key_event::Press ) );
        break;
      }

    case KeyRelease:
      {
        std::cout << "KeyRelease" << std::endl;
        XKeyEvent sym = event.xkey;
        key_code c = keys.translate_key( XLookupKeysym( &sym, 0) );

        application::get_instance().get_key_state().unset(c);
        if ( win && win->flags & window::KeyEvents )
          win->key( key_event( c, key_event::Press ) );
        break;
      }
    case ConfigureNotify:
      {
        std::cout << "ConfigureNotify" << std::endl;
        XWindowAttributes root_attribs;
        Window root_window = RootWindow( connection, screen );

        if(!win)
          return;

        XGetWindowAttributes( connection, root_window, &root_attribs );

        win->width = event.xconfigure.width;
        win->height = event.xconfigure.height;

        if((win->flags & window::Fullscreen) == 0 )
        {
          win->uwidth = win->width;
          win->uheight = win->height;
          win->px = event.xconfigure.x;
          win->py = root_attribs.height - event.xconfigure.height - event.xconfigure.y;
        }

        
        win->configure( win->width, win->height );
        break;

      }

    case Expose:
      {

        std::cout << "Expose" << std::endl;
        if( win== 0 )
          return;

        win->redraw();
        std::cout << "Expose done " << std::endl;
        break;
      }

    case ClientMessage:
      {
        std::cout << "ClientMessage" << std::endl;
        if( event.xclient.message_type == protocols_atom )
        {
          // the close event

          win->close();
        }
        else if( event.xclient.message_type == delete_atom )
        {
          std::cout << "DELETE ATOM" << std::endl;
//          _glsk_window_destroy( win);
        }
        else
        {
          std::cout << "unrecognized client-message type:" <<    XGetAtomName( connection, event.xclient.message_type ) << '\n';
        }
        break;

      };

    case UnmapNotify:
      {
        std::cout << "UnmapNotify" << std::endl;
        break;
      };

    case FocusIn:
      {
        std::cout << "FocusIn" << std::endl;
        if( (focus_window != 0) && (focus_window != win) )
          std::cout << "(gott-gui) Warning: another window is still focused.\n";

        focus_window = win;

        //_glsk_input_update_pointer_owner();
        break;

      };

    case FocusOut:
      {
        std::cout << "FocusOut" << std::endl;
        if( focus_window && (focus_window != win) )
          std::cout << "(gott-gui) Warning: losing focus without having focus.\n";

        focus_window = 0;

//        _glsk_input_update_pointer_owner();
        break;

      };

    default:
      // unrecognized event -> terminate
      std::cout << "(gott-gui) unrecognized event:" << event.type <<  '\n';
  };

}

window::os_specific::~os_specific()
{
  XDestroyWindow( global_data.connection, handle );
}

void window::set_rendercontext()
{
  if( ( global_data.glx_fallback_mode == 0 
        && !glXMakeContextCurrent( global_data.connection, os->drawable, os->drawable, context.os->handle) )
      || ( global_data.glx_fallback_mode == 1
        && !glXMakeCurrent( global_data.connection, os->drawable, context.os->handle ) ) )
    throw std::runtime_error("Error while setting context");
}

void window::swap_buffers()
{
  glXSwapBuffers( global_data.connection, os->drawable );
}

}}
