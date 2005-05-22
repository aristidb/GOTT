
#ifndef GOTT_GUI_X11_APPLICATION_HPP_INLCUDED
#define GOTT_GUI_X11_APPLICATION_HPP_INLCUDED

#include <string>
#include <set>
#include <list>
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
    std::set<std::string> extensions;
    Cursor blank_cursor;
    // actions?
    void init_extensions();
    void init_cursor();
  public:
    Application();
    Application( const char* connection );

    void register_window( Window * ref );
    void remove_window( Window *ref );

    void handle_pending_messages();

    bool is_extension_supported( std::string const& ext ) const;

    // implementation dependent methods:
    Display* get_display();
    int get_screen() const;
    bool use_fallback()const;
    Atom get_atom( char const * atom_name );
    
};

}}}

#endif

