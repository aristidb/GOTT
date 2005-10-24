#include "utility.hpp"
#include <iostream>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace fs = boost::filesystem;
namespace gott{ namespace gui{ 

  namespace detail {
bool find_font_file_rec( fs::path const& dir, std::string const& name, std::string & ret ) 
{
  fs::directory_iterator end_it;
  for( fs::directory_iterator dir_it( dir ); dir_it != end_it; ++dir_it ){
    try {
      if( fs::is_directory( *dir_it ) ) {
        if( find_font_file_rec( *dir_it, name, ret ) )
          return true;
      }else {
        if( dir_it->leaf() == name ) {
          ret = dir_it->native_file_string();
          return true;
        }
      } 
    }catch ( std::exception const& e ) {
      std::cerr << e.what();
    }
  }
  return false;
}
  }

std::string find_font_file( std::string const& path_name, std::string const& name ) 
{
  fs::path f( path_name, fs::native );
  if( fs::exists( f )  &&  fs::is_directory(f) ) {
    std::string ret;
    if( detail::find_font_file_rec( f, name, ret) )
      return ret;
  }
  return std::string();
}


rect::rect( long l, long t, std::size_t w, std::size_t h)
  : left(l), top(t), width(w), height(h)
{
}
rect::rect()
  : left(0), top(0), width(1), height(1)
{}

bool rect::is_inside( coord const& c  ) const {
  return c.x >= left && c.x <= left + long(width)  &&  c.y >= top && c.y <= top + long(height);
}
  
pixelformat::pixelformat()
  : flags( DoubleBuffer ), color(false,1), depth(false,1), samples(false,0)
{
}



}}
