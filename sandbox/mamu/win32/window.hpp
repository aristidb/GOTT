
#ifndef GOTT_GUI_X11_WINDOW_HPP_INLCUDED
#define GOTT_GUI_X11_WINDOW_HPP_INLCUDED

#include <set>
#include <string>
#include "../utility.hpp"

namespace gott{ namespace gui{ namespace x11{

class application;
class window : public gott::gui::widget_events, public gott::gui::window_flags
{
  public:
    friend class gott::gui::x11::application;
  private:
    rect window_rect;
    //gl_context context;
    application *app;
    HWND			handle;			// handle to the physical window
    HDC				dc;				// the client device context
    HGLRC			context;

    std::size_t flags;
    window * parent;

    // Implementation specific functions:

  public:
    window( application& app, rect const& r, std::string const& title, pixelformat const& p, std::size_t flags );
    window( rect const& r, std::string const& title, pixelformat const& p, std::size_t flags );

    ~window();
   
    gl_interface get_gl();
    void swap_buffer();
    
    void open( application& app, rect const&r, std::string const& title, pixelformat const& p, std::size_t flags );
    void open( rect const&r, std::string const& title, pixelformat const& p, std::size_t flags );

    void show();
    void hide();

    bool is_open() const;
    bool has_decoration() const;
    HWND get_handle() const;
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


