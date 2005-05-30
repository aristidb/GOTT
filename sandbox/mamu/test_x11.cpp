#include <stdexcept>
#include <iostream>
#include "utility.hpp"
#include "input.hpp"
#include "x11/application.hpp"
#include "x11/window.hpp"
#include "font.hpp"

using namespace std;
using namespace gott::gui;
using namespace gott::gui::x11;

class MyWindow : public gott::gui::x11::window
{
  private:
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
      : gott::gui::x11::window( app, r, title, p, window_flags::Defaults ) {
        get_face();
      }
    MyWindow( rect const& r, std::string const& title, pixelformat const& p )
      : gott::gui::x11::window( r, title, p, window_flags::Defaults ) {
        get_face();
      }

    void on_redraw() 
    {
      set_render_context();
      window::on_redraw();
      glClearColor( 0.1, 0.1, 0.1, 1.0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

     
      glColor4f(1.0,1.0,1.0,1.0);
      char text[] = "Hallo Welt";
      glRasterPos2i(-5,-5);
      int x = -5,y = -5;
      for ( int n = 0; n < sizeof(text); n++ )
      {
        FT_UInt  glyph_index;


        /* retrieve glyph index from character code */
        glyph_index = FT_Get_Char_Index( face, text[n] );
        std::cout << text[n] << " = " << glyph_index << ", ";

        /* load glyph image into the slot (erase previous one) */
        int error = FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT );
      if(error)
        throw runtime_error("Fehler bei FT_Set_Char_Size");
        /* convert to an anti-aliased bitmap */
        error = FT_Render_Glyph( face->glyph, ft_render_mode_normal );
      if(error)
        throw runtime_error("Fehler bei FT_Set_Char_Size");

      for( std::size_t i = 0; i < face->glyph->bitmap.rows; ++i )
      {
        for( std::size_t j = 0; j < face->glyph->bitmap.width; ++j )
        {
          if( face->glyph->bitmap.buffer[i * face->glyph->bitmap.width + j ]  > 0 )
            cout << '#';
          else
            cout << ' ';
          
        }
        cout << endl;
      }
     // now, draw to our target surface 
       glDrawPixels( face->glyph->bitmap.width, 
            face->glyph->bitmap.rows, 
            GL_LUMINANCE, 
            GL_UNSIGNED_SHORT,
            face->glyph->bitmap.buffer);
        // increment pen position 
       x += face->glyph->advance.x >> 6 ;
       y += face->glyph->advance.y >> 6 ; /* not useful for now */
      //  glRasterPos2i(x,y);
      }
      std::cout << std::endl;
      swap_buffer();
    }
    void on_configure( gott::gui::rect const& r)
    {
      window::on_configure(r);
      set_render_context();
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho( r.left, r.left+ r.width, r.top, r.top + r.height, 0.1, 2.0 );
    }
};


int main()
{
  try {
    application app;
    pixelformat format;
    MyWindow a_window( app, rect(0,0,200,100),"Mein Titel", format ); 
    MyWindow b_window( app, rect(300,50,200,100),"Ein anderer Titel", format); 

    app.run();
  } catch( std:: runtime_error &e ){
    std::cout << e.what()<< std::endl;
  }
}

