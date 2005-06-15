
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <GL/glu.h>
#include <freetype/ftoutln.h>
#include "vector_glyph.hpp"

using namespace std;

namespace gott{namespace gui{namespace font{

vector_glyph::vector_glyph( FT_Face & face, std::size_t glyph_index )
{
  int error = FT_Load_Glyph( face, glyph_index, FT_LOAD_NO_BITMAP );
  if(error)
    throw runtime_error("Fehler bei FT_Set_Char_Size");

  FT_Outline & outline =  face->glyph->outline;

  GLUtesselator *tobj = gluNewTess();
  gluTessCallback(tobj, GLU_TESS_BEGIN_DATA, reinterpret_cast<void(*)()>(&vector_glyph::begin_array));
  gluTessCallback(tobj, GLU_TESS_VERTEX_DATA,  reinterpret_cast<void(*)()>(&vector_glyph::add_vertex));
  gluTessCallback(tobj, GLU_TESS_END_DATA, reinterpret_cast<void(*)()>( &vector_glyph::end_array)); 
  gluTessCallback(tobj, GLU_TESS_COMBINE_DATA, reinterpret_cast<void(*)()>( &vector_glyph::combine)); 
  
  gluTessBeginPolygon(tobj, this);
  gluTessNormal(tobj, 0,0,1);

  int start = 0, end = 0;
  typedef gott::math::vector3<double> vec3;
  std::vector<vec3*> temp_array;
  
  gluTessBeginPolygon(tobj, this);
  for( int  i = 0; i < outline.n_contours; ++i )
  {
    start = end;
    end = outline.contours[i] + 1;

    
    gluTessBeginContour(tobj);
    int my_end = end;
    for( int index = start; index < my_end; ++index )
    {
      char tag = outline.tags[index];

      if( tag & 1 || (index + 1) == my_end )
      {
        temp_array.push_back( new vec3(outline.points[index].x, outline.points[index].y, 0) );
        gluTessVertex( tobj, reinterpret_cast<GLdouble*>(temp_array.back()), reinterpret_cast<void*>(temp_array.back()) );
      }
      else
      {
          std::cout << " now I was here : TAG "<< int(tag) << std::endl;
        if( temp_array.size() == 0 ) // assume that the last point in contour is an in point ... hopefully correct!
        {
          temp_array.push_back( new vec3(outline.points[my_end-1].x, outline.points[my_end-1].y, 0) );
          gluTessVertex( tobj, reinterpret_cast<GLdouble*>(temp_array.back()), reinterpret_cast<void*>(temp_array.back()) );
          --my_end;
        }
        
        v2_type control_point( outline.points[index].x, outline.points[index].y );
        v2_type previous( (*temp_array.back())[0], (*temp_array.back())[1] );

        if( !( tag & 2 ) )
        {
          std::cout << "I was here" << std::endl;
          char next_tag = ( index == my_end - 1 )
            ? outline.tags[start]
            : outline.tags[index + 1];

          v2_type data;
          if( index == my_end - 1 )  data = v2_type( (*temp_array.front())[0], (*temp_array.front())[1] );
          else data = v2_type(outline.points[index+1].x, outline.points[index+1].y);

          if( next_tag & 2 ) // further conic segments follow => median of two control points defines contour point
            data =  ( data + control_point ) * 0.5f;
          
          conic( tobj, temp_array, previous, control_point, data);
          if( index != my_end - 1 ) {
            temp_array.push_back( new vec3( data[0], data[1], 0) );
            gluTessVertex( tobj, reinterpret_cast<GLdouble*>(temp_array.back()), reinterpret_cast<void*>(temp_array.back()) );
          }

        }
        else if( tag & 2 )
        {
          std::cout << "I AM HERER " << std::endl;
          v2_type next;
          v2_type control_point_2;
          if( index == my_end - 1 ){
            control_point_2  = v2_type( (*temp_array.front())[0], (*temp_array.front())[1] );
            next = v2_type( (*(temp_array[1]))[0], (*(temp_array[1]))[1] );
          }
          else if( index == my_end -2 ) {
            control_point_2 = v2_type ( outline.points[index+1].x, outline.points[index+1].y );
            next = v2_type( (*temp_array.front())[0], (*temp_array.front())[1] );
          }
          else {
            control_point_2 = v2_type ( outline.points[index+1].x, outline.points[index+1].y );
            next = v2_type ( outline.points[index+2].x, outline.points[index+2].y );
          }

          cubic( tobj, temp_array, previous, control_point, control_point_2, next );
          if( index < my_end - 2 )
          {
            temp_array.push_back( new vec3( next[0], next[1], 0 ) );
            gluTessVertex( tobj, reinterpret_cast<GLdouble*>(temp_array.back()), reinterpret_cast<void*>(temp_array.back()) );
          }
          ++index;
        }
      }
    }
    
    gluTessEndContour(tobj);
  }
  gluTessEndPolygon(tobj);
  gluDeleteTess(tobj);

  std::size_t i = temp_array.size(); 
//  do { --i; delete temp_array[i]; } while(i);
}

void vector_glyph::conic( GLUtesselator *tobj, std::vector<gott::math::vector3<double>*> & data, v2_type const& begin, v2_type const& mid, v2_type const& end ) const
{
  for( unsigned int i = 1; i < 10; i++) 
  {
    v2_type bez1,bez2;

    float t = float(i) * 0.1;

    bez1 = (1.0f - t) * begin + t * mid;
    bez2 = (1.0f - t) * mid + t * end;
    bez1 = (1.0f - t) * bez1 + t * bez2;

    data.push_back( new gott::math::vector3<double>( bez1[0], bez1[1], 0) );
    gluTessVertex( tobj, reinterpret_cast<GLdouble*>(data.back()), reinterpret_cast<void*>(data.back()) );
  }
}

void vector_glyph::cubic( GLUtesselator *tobj, std::vector<gott::math::vector3<double>*> & data, v2_type const& begin, v2_type const& mid_1, v2_type const& mid_2, v2_type const& end ) const
{
  for( unsigned int i = 1; i < 5; i++) 
  {
    v2_type bez1,bez2, bez3;

    float t = float(i) * 0.2f;

    bez1 = (1.0f - t) * begin + t * mid_1;
    bez2 = (1.0f - t) * mid_1 + t * mid_2;
    bez3 = (1.0f - t) * mid_2 + t * end;
    bez1 = (1.0f - t) * bez1 + t * bez2;
    bez2 = (1.0f - t) * bez2 + t * bez3;
    bez1 = (1.0f - t) * bez1 + t * bez2;

    data.push_back( new gott::math::vector3<double>( bez1[0], bez1[1], 0) );
    gluTessVertex( tobj, reinterpret_cast<GLdouble*>(data.back()), reinterpret_cast<void*>(data.back()) );
  }       
}


void vector_glyph::begin_array(GLenum mode, vector_glyph *glyph) {
  glyph->array_info.push_back( mesh_info( mode, glyph->vertex_array.size(), 0 ) );
}
void vector_glyph::end_array(vector_glyph *glyph ){
}
void vector_glyph::combine(  GLdouble coords[3], void *vertex_data[4],
                                    GLfloat weight[4], void **outData,
                                    vector_glyph *glyph)
{
  std::cout << "BASTARD! combine has been called! call 911, someone has to implement it!" << std::endl;
}

void vector_glyph::add_vertex( gott::math::vector3<double>* vertex_data, vector_glyph * glyph ){
  glyph->vertex_array.push_back( v2_type( (*vertex_data)[0], (*vertex_data)[1] ) );
  ++glyph->array_info.back().count;
}

void vector_glyph::render() const
{
  float shrink = 1/16.0f;
  glPushMatrix();
  glScalef(shrink, shrink, shrink );

  glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT );
  glEnableClientState( GL_VERTEX_ARRAY );
  glDisableClientState( GL_INDEX_ARRAY );
  glDisableClientState( GL_COLOR_ARRAY );
  glDisableClientState( GL_EDGE_FLAG_ARRAY );
  glDisableClientState( GL_NORMAL_ARRAY );
  glDisableClientState( GL_TEXTURE_COORD_ARRAY );
  glVertexPointer( 2, GL_FLOAT, 0, &(vertex_array[0]) );

  for( size_t i = 0; i < array_info.size(); ++i )
    glDrawArrays(array_info[i].mode, array_info[i].first, array_info[i].count );

  glPopMatrix();
  glPopClientAttrib();

}



}}}


