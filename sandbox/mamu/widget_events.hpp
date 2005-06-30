
#include <boost/signals.hpp>
#include "utility.hpp"
#include "input.hpp"

namespace gott{namespace gui{

struct  widget_events
{
  public:
    typedef boost::signal<void()> simple_event_type;
    typedef boost::signal<void(gott::gui::rect const&)> size_signal_type;
    typedef boost::signal<void(gott::gui::mouse_event const&)> mouse_signal_type;
    typedef boost::signal<void(gott::gui::key_event const&)> key_signal_type;
  private:
    simple_event_type redraw_event, close_event, focus_enter, focus_leave;
    size_signal_type configure_event;
    mouse_signal_type mouse_ev;
    key_signal_type key_ev;

#if 0
    virtual void on_redraw() = 0;
    virtual void on_focus_enter() = 0;
    virtual void on_focus_leave() = 0;
    virtual void on_close() = 0;
    virtual void on_configure( gott::gui::rect const&) = 0;
    virtual void on_mouse(gott::gui::mouse_event const&) = 0;
    virtual void on_key(gott::gui::key_event const&) = 0;
#endif

  public:
    void set_on_redraw( simple_event_type::slot_type const& slot );
    void set_on_focus_enter( simple_event_type::slot_type const& slot );
    void set_on_focus_leave( simple_event_type::slot_type const& slot );
    void set_on_close( simple_event_type::slot_type const& slot );
    void set_on_configure( size_signal_type::slot_type const& slot );
    void set_on_mouse( mouse_signal_type::slot_type const& slot );
    void set_on_key( key_signal_type::slot_type const& slot );
    virtual ~widget_events();
  protected:
    void exec_on_redraw();
    void exec_on_focus_enter();
    void exec_on_focus_leave();
    void exec_on_close();
    void exec_on_configure( gott::gui::rect const&);
    void exec_on_mouse(gott::gui::mouse_event const&);
    void exec_on_key(gott::gui::key_event const&);
};

}}

