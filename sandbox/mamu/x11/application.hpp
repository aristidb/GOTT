
#ifndef GOTT_GUI_X11_APPLICATION_HPP_INLCUDED
#define GOTT_GUI_X11_APPLICATION_HPP_INLCUDED

#include "window.hpp"
namespace gott{ namespace gui{ namespace x11{
  
class Application
{
  private:
    Display * display;
    int screen;
    bool old_glx;
    Atom protocols_atom;
    std::list<Window*> windows;
    // actions?
  public:
    Application();
    Application( const char* connection );

    void register_window( Window * ref );
    void remove_window( Window *ref );

    bool is_extension_supported( const char* ext ) const;

    // implementation dependent methods:
    Display* get_display();
    int get_screen() const;
    bool use_fallback()const;
    Atom get_atom( char const * atom_name );
    
};

}}}

#endif

