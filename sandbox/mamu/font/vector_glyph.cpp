
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
  
  tessel_data t_dat( this );
  gluTessBeginPolygon(tobj, &t_dat);
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
        t_dat.temp_array.push_back( new vec3(outline.points[index].x, outline.points[index].y, 0) );
        gluTessVertex( tobj, reinterpret_cast<GLdouble*>(t_dat.temp_array.back()), reinterpret_cast<void*>(t_dat.temp_array.back()) );
      }
      else
      {
          std::cout << " now I was here : TAG "<< int(tag) << std::endl;
        if( t_dat.temp_array.size() == 0 ) // assume that the last point in contour is an in point ... hopefully correct!
        {
          t_dat.temp_array.push_back( new vec3(outline.points[my_end-1].x, outline.points[my_end-1].y, 0) );
          gluTessVertex( tobj, reinterpret_cast<GLdouble*>(t_dat.temp_array.back()), reinterpret_cast<void*>(t_dat.temp_array.back()) );
          --my_end;
        }
        
        v2_type control_point( outline.points[index].x, outline.points[index].y );
        v2_type previous( (*t_dat.temp_array.back())[0], (*t_dat.temp_array.back())[1] );

        if( !( tag & 2 ) )
        {
          std::cout << "I was here" << std::endl;
          char next_tag = ( index == my_end - 1 )
            ? outline.tags[start]
            : outline.tags[index + 1];

          v2_type data;
          if( index == my_end - 1 )  data = v2_type( (*t_dat.temp_array.front())[0], (*t_dat.temp_array.front())[1] );
          else data = v2_type(outline.points[index+1].x, outline.points[index+1].y);

          if( next_tag & 2 ) // further conic segments follow => median of two control points defines contour point
            data =  ( data + control_point ) * 0.5f;
          
          conic( tobj, t_dat.temp_array, previous, control_point, data);
          if( index != my_end - 1 ) {
            t_dat.temp_array.push_back( new vec3( data[0], data[1], 0) );
            gluTessVertex( tobj, reinterpret_cast<GLdouble*>(t_dat.temp_array.back()), reinterpret_cast<void*>(t_dat.temp_array.back()) );
          }

        }
        else if( tag & 2 )
        {
          std::cout << "I AM HERER " << std::endl;
          v2_type next;
          v2_type control_point_2;
          if( index == my_end - 1 ){
            control_point_2  = v2_type( (*t_dat.temp_array.front())[0], (*t_dat.temp_array.front())[1] );
            next = v2_type( (*(t_dat.temp_array[1]))[0], (*(t_dat.temp_array[1]))[1] );
          }
          else if( index == my_end -2 ) {
            control_point_2 = v2_type ( outline.points[index+1].x, outline.points[index+1].y );
            next = v2_type( (*t_dat.temp_array.front())[0], (*t_dat.temp_array.front())[1] );
          }
          else {
            control_point_2 = v2_type ( outline.points[index+1].x, outline.points[index+1].y );
            next = v2_type ( outline.points[index+2].x, outline.points[index+2].y );
          }

          cubic( tobj, t_dat.temp_array, previous, control_point, control_point_2, next );
          if( index < my_end - 2 )
          {
            t_dat.temp_array.push_back( new vec3( next[0], next[1], 0 ) );
            gluTessVertex( tobj, reinterpret_cast<GLdouble*>(t_dat.temp_array.back()), reinterpret_cast<void*>(t_dat.temp_array.back()) );
          }
          ++index;
        }
      }
    }
    
    gluTessEndContour(tobj);
  }
  gluTessEndPolygon(tobj);
  gluDeleteTess(tobj);

/*  std::size_t i = t_dat.temp_array.size(); 
  do { --i; std::cout << " i :" << i << " d:" << t_dat.temp_array[i] << std::endl; delete t_dat.temp_array[i]; } while(i);*/
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


void vector_glyph::begin_array(GLenum mode, tessel_data *d) {
  d->p->array_info.push_back( mesh_info( mode, d->p->vertex_array.size(), 0 ) );
}
void vector_glyph::end_array(tessel_data *glyph ){
}
void vector_glyph::combine( GLdouble coords[3], void *vertex_data[4], GLfloat weight[4], void **outData, tessel_data *d )
{
  std::cout << "BASTARD! combine has been called! call 911, someone has to implement it!" << std::endl;
  d->temp_array.push_back( new vec3( coords[0], coords[1], coords[2] ) );
  /*new->r = w[0]*d[0]->r + w[1]*d[1]->r + w[2]*d[2]->r + w[3]*d[3]->r;
  new->g = w[0]*d[0]->g + w[1]*d[1]->g + w[2]*d[2]->g + w[3]*d[3]->g;
  new->b = w[0]*d[0]->b + w[1]*d[1]->b + w[2]*d[2]->b + w[3]*d[3]->b;
  new->a = w[0]*d[0]->a + w[1]*d[1]->a + w[2]*d[2]->a + w[3]*d[3]->a;*/
  *outData = d->temp_array.back();
}

void vector_glyph::add_vertex( gott::math::vector3<double>* vertex_data, tessel_data* d){
  d->p->vertex_array.push_back( v2_type( (*vertex_data)[0], (*vertex_data)[1] ) );
  ++d->p->array_info.back().count;
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


