#include <stdexcept>
#include <iostream>
#include <GL/glu.h>
#include "utility.hpp"
#include "input.hpp"
#include "x11/application.hpp"
#include "x11/window.hpp"
#include "font.hpp"
#include "font/vector_glyph.hpp"

using namespace std;
using namespace gott::gui;
using namespace gott::gui::font;
using namespace gott::gui::x11;

class MyWindow : public gott::gui::x11::window
{
  private:
    float x_,y_;
    FT_Face face;
    void get_face() {
      int error = FT_New_Face( gott::gui::font::get_instance(),
          "/usr/X11R6/share/fonts/corefonts/arial.ttf",
          0,
          &face );
      if( error )
        throw runtime_error("Fehler bei FT_New_Face");
      error = FT_Set_Char_Size(
            face,    /* handle to face object           */
            0,       /* char_width in 1/64th of points  */
            16*64,   /* char_height in 1/64th of points */
            300,     /* horizontal device resolution    */
            300 ); 
      if(error)
        throw runtime_error("Fehler bei FT_Set_Char_Size");

    }
  public:
    MyWindow( application& app, rect const& r, std::string const& title, pixelformat const& p )
      : gott::gui::x11::window( app, r, title, p, window_flags::Defaults ), x_(0.01), y_(0.01) {
        get_face();
      }
    MyWindow( rect const& r, std::string const& title, pixelformat const& p )
      : gott::gui::x11::window( r, title, p, window_flags::Defaults ), x_(0.01), y_(0.01){
        get_face();
      }

    void on_key( gott::gui::key_event const& ev )
    {
      using namespace gott::gui;
      if(ev.type == gott::gui::key_event::Press )
      {
        switch(ev.code)
        {
          case KeyLeft: x_-=1.01; break;
          case KeyRight: x_+=1.01; break;
          case KeyUp: y_-=1.01; break;
          case KeyDown: y_+=1.01; break;
          default: break;
        }
      }
    }

    void on_redraw() 
    {
      set_render_context();
      window::on_redraw();
      glClearColor( 0.8, 0.3, 0.2, 1.0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glLoadIdentity();
  

      // pregenerated data:
      glColor4f(1.0,1.0,1.0,1.0);
#if 0
      glBegin( GL_TRIANGLE_STRIP );
      glVertex2d(3.62154e-312, 4.46765e-308);
      glVertex2d(2.86558e-322, 3.62132e-312);
      glVertex2d(12, 0);
      
      glVertex2d(34, 0);
      glVertex2d(24.66, 15.5881);
      glVertex2d(26.4219, 1.70312);
      
      glVertex2d(-2.30968e-07, 2.4729e-316);
      glVertex2d(11, 0);
      glVertex2d(4.44659e-323, 3.31562e-316);
      
      glVertex2d(-2.30968e-07, 2.4729e-316);
      glVertex2d(11, 0);
      glVertex2d(4.44659e-323, 3.31562e-316);
      
      glVertex2d(7.28125, 3.76562);
      glVertex2d(9.10062, 2.05);
      glVertex2d(9.27063, 4.3375);
      glVertex2d(11.1337, 0.715625);
      
      glVertex2d(13.1013, 2.24062);
      glVertex2d(13.3806, -0.2375);
      glVertex2d(0, 0);
      glVertex2d(15.8413, -0.809375);
      glVertex2d(7.02375, 5.29062);
//      glVertex2d(6.36063, 5.1);
      glEnd( );
#endif
      
      glBegin(GL_TRIANGLES);
      glColor3f(1.0,0,0);
      glVertex2i( 200,250);
      glColor3f(0,1,0);
      glVertex2i( 50,50);
      glColor3f(0,0,1);
      glVertex2i( 500,30);
      glEnd();
      
      glColor4f(1.0,1.0,1.0,1.0);
      char text[] = "Hallo Welt";
      glTranslatef(0,0,0.25);
      for ( std::size_t n = 0; n < sizeof(text); n++ )
      {
        GLenum e = glGetError();
        if( e != GL_NO_ERROR )
          std::cout << gluErrorString( e ) << std::endl;
        vector_glyph g( face, FT_Get_Char_Index( face, text[n] ) );
        g.render();
        glTranslatef( (face->glyph->advance.x << 6 ), -(face->glyph->advance.y << 6 ), 0 );
      }
 
      swap_buffer();
    }
    void on_configure( gott::gui::rect const& r)
    {
      window::on_configure(r);
      set_render_context();
      glViewport( 0, 0,  r.width, r.height);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
   //   gluOrtho2D( 0, r.width, 0, -r.height );
      gluOrtho2D( 0, r.width, r.height,0 );
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glDisable(GL_CULL_FACE );
      glDisable(GL_TEXTURE_2D);
      glDisable(GL_LIGHTING );
      glDisable(GL_DEPTH_TEST);
      glDisable(GL_TEXTURE_2D);
      glDisable(GL_BLEND);
      glDisable(GL_ALPHA_TEST);
      glDisable(GL_STENCIL_TEST);

    }
};


int main()
{
  try {
    application app;
    pixelformat format;
    MyWindow a_window( app, rect(0,0,200,100),"Mein Titel", format ); 

    app.run();
  } catch( std:: runtime_error &e ){
    std::cout << e.what()<< std::endl;
  }
}

