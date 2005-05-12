
#ifndef GOTT_GUI_WINDOW_HPP_INCLUDED
#define GOTT_GUI_WINDOW_HPP_INCLUDED

#include <boost/signal.hpp>
#include "application.hpp"
#include "gl_context.hpp"
#include "pixelformat.hpp"

namespace gott{namespace gui{
struct mouse_event{ };
class window
{
  public:
    friend class gott::gui::application;
    friend class gott::gui::globals;

    typedef boost::signal<void(std::string const&)> key_signal_type; 
    typedef boost::signal<void()> simple_event_type;
    typedef boost::signal<void(int, int)> size_signal_type;
    typedef boost::signal<void(gott::gui::mouse_event const&)> mouse_signal_type;
    // boost::signal<void,glsk::pixelformat& >&    signal_pixelformat_select();

    enum Flags {
      Clear = 0
        , Decoration
        , Fullscreen
        , Visible = 4
        , Open = 8
    };
  private:
    struct os_specific;
    os_specific *os;
    int width
      , height
      , uwidth
      , uheight
      , px
      , py;

    std::size_t flags;

    gl_context context;

    std::string title; 
    key_signal_type key_event;
    simple_event_type redraw_event, idle_event, destroy_event, close_event;
    size_signal_type configure_event;
    mouse_signal_type mouse_ev;

    window( window const& );
    window& operator=( window const& );
  public:
    window( gl_context const& con, pixelformat const& format, std::size_t flags );
    virtual ~window();

    void set_decoration( bool decor = true );
    void set_visible( bool visible = true );
    void set_title( std::string const& title );
    void set_position( int x, int y );
    void set_size( std::size_t w, std::size_t h );
    void set_rendercontext();

    void set_on_key( key_signal_type::slot_type const& slot );
    void set_on_redraw( simple_event_type::slot_type const& slot );
    void set_on_close( simple_event_type::slot_type const& slot );
    void set_on_destroy( simple_event_type::slot_type const& slot );
    void set_on_idle( simple_event_type::slot_type const& slot );
    void set_on_configure( size_signal_type::slot_type const& slot );
  //  void set_on_create( size_signal_type::slot_type const& slot );
    void set_on_mouse( mouse_signal_type::slot_type const& slot );

  private:
    virtual void on_key( std::string const& );
    virtual void on_redraw();
    virtual void on_configure( int,int);
    virtual void on_create( int,int);
    virtual void on_close();
    virtual void on_destroy();
    virtual void on_idle();
    virtual void on_mouse_event(gott::gui::mouse_event const&);
    
    void key( std::string const& );
    void redraw();
    void configure( int,int);
    void create( int,int);
    void close();
    void destroy();
    void idle();
    void mouse(gott::gui::mouse_event const&);

};
}}

#endif
