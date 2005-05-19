
#include "x11window.hpp"
namespace gott{ namespace gui{ namespace x11{
  
class Application
{
  private:
    Display * display;
    int screen;
    bool old_glx;
    XAtom protocols_atom;
    std::list<Window*> windows;
    // actions?
  public:
    Application();
    Application( const char* connection );

    void register_window( Window * ref );
    
};

}}}

