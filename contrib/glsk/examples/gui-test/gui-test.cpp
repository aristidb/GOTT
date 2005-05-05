// $Id: glskmm-test.cpp,v 1.2 2004/12/19 00:53:02 ltjax Exp $
#include <GL/gl.h>
#include <GL/glu.h>
#include <glsk/glskmm.h>
#include <cmath>

#include <boost/array.hpp>
#include <boost/signal.hpp>
#include <boost/mpl/void.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/noncopyable.hpp>
#include <vector>
#include <iostream>

struct surface { };

typedef unsigned long coord_t;

struct coord {
  coord_t x,y;
  coord(coord_t x_, coord_t y_) : x(x_), y(y_) { }
  coord() { }
};

bool operator<(coord const &a, coord const &b) {
  return a.x<b.x && a.y<b.y;
}

std::ostream &operator<<(std::ostream &o, coord const &c) {
  o << '(' << c.x << ", " << c.y << ')';
  return o;
}

//typedef unsigned long size_t;

struct size {
  size_t width, height;
  size(size_t w, size_t h) : width(w), height(h) { }
  size() { }
};

coord operator+(coord const &c, size const &s) {
  return coord(c.x+s.width, c.y+s.height);
}

struct rectangle {
  coord c;
  size s;

  rectangle(coord const &c_, size const &s_) : c(c_), s(s_) { }
  rectangle() { }
};
//----

// is_parent.hpp { <done>
template<class T>
struct is_parent {
  enum { value=false };
};
// }

// mouse_button_event.hpp { <done>
namespace mouse {
  class button_event {
    coord c;
    unsigned int button;
    public:
    button_event(coord const &c_, unsigned int b)
      : c(c_), button(b)
      { }

    coord const &get_coord() const { return c; }
    unsigned int get_button() const { return button; }
  };
}
// }

// value2type.hpp {
template<int Value, typename Type>
struct value2type {
  typedef Type type;
  enum { value=Value };
};
// }

// event_mapper.hpp {
template<class Events>
class event_mapper {
  template<std::size_t n=0, bool is_end=
    boost::tuples::length<Events>::value==n>
    struct event_handler {
      typedef typename boost::tuples::element<n, Events>::type entry_t;
      enum { signal_nr=entry_t::value };
      boost::signal<typename entry_t::type> sig;

      typedef event_handler<n+1> next_t;
      next_t next;
    };

  template<std::size_t n>
    struct event_handler<n, true> { };

  template<int sig_nr, class Functor, class Handler,
    bool is_sig= sig_nr==Handler::signal_nr>
      struct set_handler_ {
        boost::signals::connection operator()(Functor f, Handler &h) {
          set_handler_<sig_nr, Functor, typename Handler::next_t> sh;
          return sh(f,h.next);
        }
      };

  template<int sig_nr, class Functor, class Handler>
    struct set_handler_<sig_nr, Functor, Handler, true> {
      boost::signals::connection operator()(Functor f, Handler &h) {
        return h.sig.connect(f);
      }
    };

  template<int sig_nr, class Callback, class Handler,
    bool is_sig= sig_nr==Handler::signal_nr>
      struct get_ {
        boost::signal<Callback> &operator()(Handler &h) {
          get_<sig_nr, Callback, typename Handler::next_t> sh;
          return sh(h.next);
        }
      };

  template<int sig_nr, class Callback, class Handler>
    struct get_<sig_nr, Callback, Handler, true> {
      boost::signal<Callback> &operator()(Handler &h) {
        return h.sig;
      }
    };

  event_handler<> handler_;
  public:
  template<int sig_nr, class Functor>
    boost::signals::connection set_handler(Functor f) {
      set_handler_<sig_nr, Functor, event_handler<> > sg;
      return sg(f, handler_);
    }

  template<int sig_nr, class Callback>
    boost::signal<Callback> &get() {
      get_<sig_nr, Callback, event_handler<> > sg;
      return sg(handler_);
    }
};
// }

// window_style.hpp {
struct window_style {
  class drawingarea {
    coord c;
    size s;
    public:
    void resize(size const &s_) { s=s_; }
    void move(coord const &c_)  { c=c_; }

    coord const &get_coord() const { return c; }
    size const &get_size() const { return s; }
  };

  void draw(surface &) const { }
  drawingarea const &get_drawingarea() const { return darea; }
  drawingarea &get_drawingarea()             { return darea; }
  private:
  drawingarea darea;
};
// }

// window.hpp {
template<class Style=window_style, template<class> class Events=event_mapper>
struct window {
  struct event {
    enum { destroyed, moved, resized };
  };
  typedef Events<boost::tuple<value2type<event::destroyed,void ()>,
          value2type<event::moved,void (coord const&)>,
          value2type<event::resized,void (size const&)>
            > > event_t;
  typedef Style style_t;
  typedef typename Style::drawingarea drawingarea;

  window(rectangle const &r_)
    : r(r_)
    {
      style_.get_drawingarea().move(r.c);
      style_.get_drawingarea().resize(r.s);
    }
  template<class StyleTmp>
    window(rectangle const &r_, StyleTmp const &o)
    : r(r_), style_(o)
    {
      style_.get_drawingarea().move(r.c);
      style_.get_drawingarea().resize(r.s);
    }
  ~window() { events_.template get<event::destroyed, void ()>()(); }

  event_t &events() { return events_; }

  drawingarea const &get_drawingarea() const {
    return style_.get_drawingarea();
  }
  void draw(surface &to) const { style_.draw(to); }

  style_t &style()             { return style_; }
  style_t const &style() const { return style_; }

  coord const &get_coord() const { return r.c; }
  void move(coord const &c) {
    r.c=c;
    style_.get_drawingarea().move(c);
    events_.template get<event::moved, void (coord const&)>()(c);
  }

  size const &get_size() const { return r.s; }
  void resize(size const &s) {
    r.s=s;
    style_.get_drawingarea().resize(s);
    events_.template get<event::resized, void (size const&)>()(s);
  }

  template<class Event> void add_event(Event const &);
  void add_event(mouse::button_event const &e) { button_events.push_back(e); }

  template<class Widgets>
    void dispatch_events(Widgets &w) const {
      if(!button_events.empty())
        for(std::vector<mouse::button_event>::const_iterator i=
            button_events.begin();
            i!=button_events.end();
            ++i)
          w.template get<0>().dispatch_event(*i, w.get_tail());
    }
  private:
  //TODO is a vector realy necessary?
  std::vector<mouse::button_event> button_events;

  rectangle r;
  event_t events_;
  style_t style_;
};
// }

// vbox.hpp {
class vbox {
  rectangle r;

  template<class Tuple, bool is_p=is_parent<typename Tuple::head_type>::value>
  struct handle_smaller_call {
    template<typename Iterator>
    handle_smaller_call(Tuple &t, std::size_t &n, size_t k, size_t &height,
			size_t width, Iterator done)
    {
      handle_smaller(t.get_tail(), n, k, height, width, done+1);
    }
  };

  template<class Tuple>
  struct handle_smaller_call<Tuple, true> {
    template<typename Iterator>
    handle_smaller_call(Tuple &t, std::size_t &n, size_t k, size_t &height,
			size_t width, Iterator done)
    {
      handle_smaller(t.get_tail().get_tail(), n, k, height, width, done+2);
    }
  };

  template<class Tuple, typename Iterator>
  static void handle_smaller(Tuple &t, std::size_t &n, size_t k,
			     size_t &height, size_t width, Iterator done)
  {
    typedef typename Tuple::head_type widget_type;
    if(!*done) {
      widget_type &w=t.template get<0>();
      if(w.get_size().height <= k && w.get_size().height != 0) {
	--n;
	height-=w.get_size().height;
	*done=true;
	if(w.get_size().width > width || w.get_size().width == 0)
	  w.resize(size(width, w.get_size().height));
      }
    }
    handle_smaller_call<Tuple> m(t, n, k, height, width, done);
  }

  template<typename Iterator>
  static void handle_smaller(boost::tuples::null_type,std::size_t,size_t,
			     size_t,size_t,Iterator) { }

  template<class Tuple, bool is_p=is_parent<typename Tuple::head_type>::value>
  struct resize_rest_call {
    template<typename Iterator>
    resize_rest_call(Tuple &t, Iterator done, size_t k, size_t width) {
      resize_rest(t.get_tail(), done+1, k, width);
    }
  };

  template<class Tuple>
  struct resize_rest_call<Tuple, true> {
    template<typename Iterator>
    resize_rest_call(Tuple &t, Iterator done, size_t k, size_t width) {
      resize_rest(t.get_tail().get_tail(), done+2, k, width);
    }
  };

  template<class Tuple, typename Iterator>
  static void resize_rest(Tuple &t, Iterator done, size_t k, size_t width) {
    typedef typename Tuple::head_type widget_type;
    if(!*done) {
      widget_type &w=t.template get<0>();
      size newsize(width, k);
      if(w.get_size().width != 0 && w.get_size().width < width)
	newsize.width=w.get_size().width;
      w.resize(newsize);
    }
    resize_rest_call<Tuple>(t, done+1, k, width);
  }

  template<typename Iterator>
  static void resize_rest(boost::tuples::null_type, Iterator, size_t, size_t)
  { }

  template<class Tuple, bool is_p=is_parent<typename Tuple::head_type>::value>
  struct draw_them_call {
    draw_them_call(Tuple &t, surface &to, coord const &c, size_t n) {
      draw_them(t.get_tail(), to, c, n);
    }
  };

  template<class Tuple>
  struct draw_them_call<Tuple, true> {
    draw_them_call(Tuple &t, surface &to, coord const &c, size_t n) {
      t.template get<0>().draw_widgets(t.template get<1>(), to);
      draw_them(t.get_tail().get_tail(), to, c, n);
    }
  };

  template<class Tuple>
  static void draw_them(Tuple &t, surface &to, coord const &c, size_t n=0) {
    typedef typename Tuple::head_type widget_type;
    widget_type &w=t.template get<0>();

    coord c_(c.x, c.y+n);
    w.draw(c_, to);
    draw_them_call<Tuple>(t, to, c, n+w.get_size().height);
  }

  static void draw_them(boost::tuples::null_type,surface&,coord const&,size_t)
  { }

  template<class Tuple, bool is_p=is_parent<typename Tuple::head_type>::value>
  struct handle_dispatch_event_call {
    template<class Event>
    handle_dispatch_event_call(Event const &e, Tuple &t) {
      handle_dispatch_event(e, t.get_tail());
    }
  };

  template<class Tuple>
  struct handle_dispatch_event_call<Tuple, true> {
    template<class Event>
    handle_dispatch_event_call(Event const &e, Tuple &t) {
      handle_dispatch_event(e, t.get_tail().get_tail());
    }
  };

  template<class Event, class Widgets>
  static void handle_dispatch_event(Event const &e, Widgets &w) {
    if(w.template get<0>().get_coord() < e.get_coord() &&
       e.get_coord() <
       w.template get<0>().get_coord() + w.template get<0>().get_size())
      w.template get<0>().dispatch_event(e, w.get_tail());

    handle_dispatch_event_call<Widgets>(e,w);
  }

  template<class Event>
  static void handle_dispatch_event(Event const&, boost::tuples::null_type)
  { }
public:
  size const &get_size() const        { return r.s; }
  void resize(size const &s)          { r.s=s; }
  coord const &get_coord() const      { return r.c; }
  void move(coord const &c)           { r.c=c; }
  void draw(coord const &c, surface&) { r.c=c; }

  template<class Tuple>
  void draw_widgets(Tuple &t, surface &to) {
    typedef boost::array<bool,boost::tuples::length<Tuple>::value> array_t;
    array_t done;
    std::fill(done.begin(), done.end(), false);

    std::size_t n=boost::tuples::length<Tuple>::value;
    size_t height=r.s.height;
    size_t k=height/n;
    for(;;) {
      handle_smaller(t, n, k, height, r.s.width, done.begin());
      if(!n)
	break;
      size_t k_=height/n;
      if(k==k_)
	break;
      k=k_;
    }
    resize_rest(t, done.begin(), k, r.s.width);
    draw_them(t, to, r.c);
  }

  template<class Event, class Widgets>
  void dispatch_event(Event const &e, Widgets &w) {
    handle_dispatch_event(e, w.template get<0>());
  }
};

template<>
struct is_parent<vbox> {
  enum { value=true };
};
// }

// update.hpp {
namespace helper {
  class update : boost::noncopyable {
    surface &to;

    template<bool is_parent, class Widgets>
      struct handle_parent {
        handle_parent(Widgets &, surface) { }
      };

    template<class Widgets>
      struct handle_parent<true, Widgets> {
        handle_parent(Widgets &w, surface to) {
          w.template get<0>().draw_widgets(w.template get<1>(), to);
        }
      };

    template<class Window>
      void draw_widgets(Window const &, boost::tuples::null_type) const { }

    template<class Window, class Widgets>
      void draw_widgets(Window const &w, Widgets &t_) const {
        typedef typename boost::tuples::element<0,Widgets>::type widget_type;
        handle_parent<is_parent<widget_type>::value, Widgets> m(t_,to);
        draw_widgets(w, t_.get_tail());
      }
    public:
    update(surface &t) : to(t) { }

    template<class Window, class Widgets>
      void draw(Window const &w, Widgets &t) const {
        typedef typename boost::tuples::element<0,Widgets>::type widget_type;
        assert( is_parent<widget_type>::value );
        t.template get<0>().resize(w.get_drawingarea().get_size());
        t.template get<0>().move(w.get_drawingarea().get_coord());
        draw_widgets(w, t);

        w.dispatch_events(t);
      }
  };
}

template<class Window, class Widgets>
void draw(Window const &w, Widgets &t, surface &to) {
  helper::update u(to);
  u.draw(w,t);
}
// }

  struct widget0 {
    widget0(size const &s_=size(150,200))
      : r(coord(),s_)
      { }

    size const &get_size() const { return r.s; }
    void resize(size const &s_) { r.s=s_; }

    coord const &get_coord() const { return r.c; }

    void draw(coord const &c, surface) {
      r.c=c;
      glColor3f(1.0,0.0,0.0);
      glBegin(GL_QUADS);
        glVertex2i( r.c.x + 1, r.c.y + 1 );
        glVertex2i( r.c.x + r.s.width - 1, r.c.y + 1 );
        glVertex2i( r.c.x + r.s.width - 1, r.c.y + r.s.height - 1);
        glVertex2i( r.c.x + 1, r.c.y + r.s.height - 1);
      glEnd();
      std::cout << "((" << r.c.x << ", " << r.c.y << ") (" << r.s.width
        << ", " << r.s.height << "))\n";
    }

    template<class Event, class Void>
      void dispatch_event(Event const &,Void const &) { }

    template<class Void>
      void dispatch_event(mouse::button_event const &e, Void const &) {
        std::cout << "Button: " << e.get_button() << "@(" << e.get_coord().x
          << ", " << e.get_coord().y << ")\n";
      }
    private:
    rectangle r;
  };

void foo() {
  std::cout << "FOO\n";
}

class testwindow : public glsk::window
{
  private:
  int			mx;
  int			my;
  boost::tuple<vbox, boost::tuple<widget0, vbox,
				  boost::tuple<widget0, widget0>, widget0 > > widgets;
  //     boost::tuple<vbox, boost::tuple<widget0, widget0> > widgets;


    surface to;

    ::window<> win;


  public:
    testwindow( float radius );

    void		on_char( std::string& value );
    void		on_redraw();
    bool		on_configure( int x, int y );
    bool		on_create( int x, int y );
    void		on_pixelformat_select( glsk::pixelformat& format );
    void		on_mouse_event( const glsk::mouse_event& mouse_event );
    void		on_idle();

    void		update();
    void		render();
};

#include <iostream>

testwindow::testwindow( float radius )
  : win(rectangle(coord(0,0), size(400,300)))
{
  win.events().set_handler< ::window<>::event::destroyed>(foo);
  set_idle_handler( true );
}

void testwindow::on_mouse_event( const glsk::mouse_event& mouse_event )
{
  switch( mouse_event.get_type() )
  {
    case glsk::mouse_event::move:
      {

      };
      break;

    case glsk::mouse_event::button_pressed:
      win.add_event(mouse::button_event(coord( mouse_event.get_position()[ 0 ], mouse_event.get_position()[ 1 ]), mouse_event.get_button() ));
      if ( mouse_event.get_button() == 3 )
        this->destroy();

      break;

    default:
      break;
  }
}

void testwindow::update()
{
}

void testwindow::render()
{



  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glLoadIdentity();
  glColor3ub( 0, 0, 0 );
  draw(win, widgets, to);


  
  this->swap_buffers();
}

void testwindow::on_redraw()
{
  render();
}

void testwindow::on_idle()
{
  update();

  render();
}

bool testwindow::on_configure( int x, int y )
{
  glClearColor( 0.86f, 0.93f, 1.f, 1.f );

  mx = x/2;
  my = y/2;

  glViewport( 0, 0, x, y );

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();

  gluOrtho2D( 0, x, y, 0 );

  glMatrixMode( GL_MODELVIEW );

  glPointSize( 3.f );
  glLineWidth( 3.f );

  return true;
}

bool testwindow::on_create( int x, int y )
{
  return select_rendercontext();
}

void testwindow::on_pixelformat_select( glsk::pixelformat& format )
{
  format.set_flags( glsk::pf::draw_to_window | glsk::pf::doublebuffer |
      glsk::pf::color_bits_set | glsk::pf::depth_bits_set | glsk::pf::multisample );

  format.set_colorbits( 32 );
  format.set_depthbits( 16 );
  format.set_samples( 4 );
}

void testwindow::on_char( std::string& string )
{
}

int glsk::user_main( int argc, char** argv )
{
	glsk::context		context;
  testwindow wnd( 5.f );

  wnd.set_title( "Title" );
   wnd.set_decorated( true );
  wnd.set_size( 400, 300 );
//  wnd.set_position( 200, 150 );

  if ( wnd.open(context) && wnd.show() )
  {
    return glsk::main::run();
  }
  else
  {
    glsk::error_box( "unable to show window" );
  }

  return 0;
}
