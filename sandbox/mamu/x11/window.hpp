
#ifndef GOTT_GUI_X11_WINDOW_HPP_INLCUDED
#define GOTT_GUI_X11_WINDOW_HPP_INLCUDED

#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/extensions/xf86vmode.h>
#include <set>
#include <string>
#include "../widget_events.hpp"

namespace gott{ namespace gui{ namespace x11{

class application;
class window : public gott::gui::widget_events, public gott::gui::window_flags
{
  public:
    friend class gott::gui::x11::application;
  private:
    rect window_rect;
    application *app;
    ::Window		handle;	
    std::size_t flags;
    enum Protocols { DeleteWindow, Focus, Ping, ContextHelp };
    ::Atom protocols[4];
    ::Atom wm_name, wm_icon_name, wm_type;
    window * parent;

    // Implementation specific functions:

    XVisualInfo* get_visualinfo( pixelformat const& p ) const;
  public:
    window( application& app, rect const& r, std::string const& title, pixelformat const& p, std::size_t flags );
    window( rect const& r, std::string const& title, pixelformat const& p, std::size_t flags );

    ~window();
   
    void swap_buffer();
    
    void open( application& app, rect const&r, std::string const& title, pixelformat const& p, std::size_t flags );
    void open( rect const&r, std::string const& title, pixelformat const& p, std::size_t flags );

    void show();
    void hide();

    bool is_open() const;
    bool has_decoration() const;
    ::Window get_handle() const;
    rect const& get_rect() const;

    void close();
    
    void set_window_type( std::size_t flags );
    void set_visible( bool vis );
    void set_title( std::string const& title );
    void set_region( rect const& region );
    void set_render_context();


    // event handlers:
    virtual void on_redraw();
    virtual void on_focus_enter();
    virtual void on_focus_leave();
    virtual void on_close();
    virtual void on_configure( gott::gui::rect const&);
    virtual void on_mouse(gott::gui::mouse_event const&);
    virtual void on_key(gott::gui::key_event const&);

};

}}}

#endif


