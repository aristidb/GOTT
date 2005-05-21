
#ifndef GOTT_GUI_X11_WINDOW_HPP_INLCUDED
#define GOTT_GUI_X11_WINDOW_HPP_INLCUDED

#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <X11/extensions/xf86vmode.h>
#include <set>
#include <string>
#include "utility.hpp"

namespace gott{ namespace gui{ namespace x11{

class Application;
class Window : public gott::gui::SignalPack, public gott::gui::WindowFlags
{
  public:
  private:
    rect window_rect;
    //gl_context context;
    Application *app;
    ::Window		handle;	
    GLXContext  context;
    GLXWindow		drawable;
    std::size_t flags;
    Atom protocols[4];
    Atom wm_name, wm_icon_name, wm_type;

    // Implementation specific functions:

    GLXFBConfig get_fbconfig( pixelformat const& p ) const;
    XVisualInfo* get_visualinfo( pixelformat const& p ) const;
  public:
    Window( Application& app, rect const& r, std::string const& title, pixelformat const& p, std::size_t flags );
    Window( rect const& r, std::string const& title, pixelformat const& p, std::size_t flags );

    ~Window();
   
    gl_interface get_gl();
    
    void open( Application& app, rect const&r, std::string const& title, pixelformat const& p, std::size_t flags );
    void open( rect const&r, std::string const& title, pixelformat const& p, std::size_t flags );

    void show();
    void hide();

    bool is_open() const;

    void close();
    
    void set_window_type( std::size_t flags );
    void set_visible( bool vis );
    void set_title( std::string const& title );
    void set_region( rect const& region );
};

}}}

#endif


