#include <stdexcept>
#include <iostream>
#include <GL/glu.h>
#include "utility.hpp"
#include "input.hpp"
#include "x11/application.hpp"
#include "x11/window.hpp"
#include "font.hpp"
#include "font/pixel_glyph.hpp"
#include "font/vector_glyph.hpp"
#include "device.hpp"
#include <boost/bind.hpp>

using namespace std;
using namespace gott::gui;
using namespace gott::gui::font;
using namespace gott::gui::x11;

class MyWindow : public gott::gui::x11::window
{
  private:
    float x_,y_;
    device_property dev;
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
            64*20,       /* char_width in 1/64th of points  */
            64*20,   /* char_height in 1/64th of points */
            96,     /* horizontal device resolution    */
            96 ); 
      if(error)
        throw runtime_error("Fehler bei FT_Set_Char_Size");

    }
  public:
    MyWindow( application& app, rect const& r, std::string const& title, pixelformat const& p )
      : gott::gui::x11::window( app, r, title, p, window_flags::Defaults ), x_(0.01), y_(10.01) {
        set_on_key(boost::bind(&MyWindow::on_key, this, _1));
        set_on_redraw(boost::bind(&MyWindow::on_redraw, this));
        set_on_configure(boost::bind(&MyWindow::on_configure, this, _1));
        dev.dot_layout = device_property::LCDHorizontal;//device_property::CRTDiffuse;
        dev.v_dpi = 96;
        dev.h_dpi = 96;
        get_face();
      }
    MyWindow( rect const& r, std::string const& title, pixelformat const& p )
      : gott::gui::x11::window( r, title, p, window_flags::Defaults ), x_(0.01), y_(10.01){
        set_on_key(boost::bind(&MyWindow::on_key, this, _1));
        set_on_redraw(boost::bind(&MyWindow::on_redraw, this));
        set_on_configure(boost::bind(&MyWindow::on_configure, this, _1));
        dev.dot_layout = device_property::LCDHorizontal;//device_property::CRTDiffuse;
        dev.v_dpi = 96;
        dev.h_dpi = 96;
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
      glClearColor( 0.8, 0.8, 0.8, 1.0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glLoadIdentity();

      glBegin(GL_TRIANGLES);
      glColor3f(1.0,0,0);
      glVertex2i( 400,450);
      glColor3f(0,1,0);
      glVertex2i( 250,250);
      glColor3f(0,0,1);
      glVertex2i( 700,230);
      glEnd();
      
      char text[] = "A library to rule them all!";
      //glTranslatef(0,0,0.25);
      glRasterPos2f( x_, y_ );
      float t_x = x_, t_y = y_;
      for ( std::size_t n = 0; n < sizeof(text) - 1; n++ )
      {
        GLenum e = glGetError();
        if( e != GL_NO_ERROR )
          std::cout << gluErrorString( e ) << std::endl;
        pixel_glyph g( face, FT_Get_Char_Index( face, text[n] ), dev);
        g.render();
        t_x += face->glyph->advance.x/32.0f;
        t_y += face->glyph->advance.y/32.0f;
        glRasterPos2f( t_x, t_y );
      }
      glColor4f(0.2,0.2,1.20,1.0);
      glTranslatef(0,100,0.0);
      for ( std::size_t n = 0; n < sizeof(text) - 1; n++ )
      {
        GLenum e = glGetError();
        if( e != GL_NO_ERROR )
          std::cout << gluErrorString( e ) << std::endl;
        vector_glyph g( face, FT_Get_Char_Index( face, text[n] ) );
        g.render();
        glTranslatef( (face->glyph->advance.x/16.0f ), (face->glyph->advance.y/16.0f), 0 );
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
      gluOrtho2D( 0, r.width, 0, r.height );
//      gluOrtho2D( 0, r.width, r.height,0 );
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glDisable(GL_CULL_FACE );
      glDisable(GL_TEXTURE_2D);
      glDisable(GL_LIGHTING );
      glDisable(GL_DEPTH_TEST);
      glDisable(GL_TEXTURE_2D);
      glDisable(GL_ALPHA_TEST);
      glEnable(GL_BLEND);
      glDisable(GL_STENCIL_TEST);
      glEnable(GL_POINT_SMOOTH);
      glPointSize( 2.0f );
      glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

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

