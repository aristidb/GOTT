
#ifndef GOTT_GUI_APPLICATION_HPP_INCLUDED
#define GOTT_GUI_APPLICATION_HPP_INCLUDED

#include <boost/signal.hpp>
#include <list>

namespace gott{ namespace gui{
class globals;
class window;
class application 
{
  public:
    friend class gott::gui::globals;

    typedef boost::signal<void ()> idle_signal;
    enum status {  Continue, End};
    typedef std::list<window*>::iterator window_iterator;
  private:
    application();
    application( application const& );
    application& operator=( application const& );

    idle_signal process_idle;
    std::list<window*> windows;
  public:
    static application& get_instance();
    
    window_iterator begin();
    window_iterator end();
    void run();
    void quit();
    status handle_pending_messages();
    void handle_idle();
    boost::signals::connection append_process_idle_handler( idle_signal::slot_type const& slot );

    void add_window( window * );
    void remove_window( window * );
};

}}


#endif

