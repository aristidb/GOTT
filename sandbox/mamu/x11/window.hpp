
namespace gott{ namespace gui{ namespace x11{

class Application;
class Window : public gott::gui::SignalPack, public gott::gui::WindowFlags
{
  public:
  private:
    rect window_rect;
    gl_context context;
    Application *app;

  public:
    Window( Application& app, rect const& r, std::string const& title, pixelformat const& p, std::size_t flags );
    Window( rect const& r, std::string const& title, pixelformat const& p, std::size_t flags );

    ~Window();
   
    gl_interface get_gl();
    
    void open( Application& app, rect const&r, std::string const& title, pixelformat const& p, std::size_t flags );
    void open( rect const&r, std::string const& title, pixelformat const& p, std::size_t flags );

    void show();
    void hide();

    void close();
    
    void set_window_type( std::size_t flags );
    void set_visible( bool vis );
    void set_title( std::string const& title );
    void set_region( rect const& region ):

    ~Window();
    
};

}}}
