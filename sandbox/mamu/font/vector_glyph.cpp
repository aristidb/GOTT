
#include <stdexcept>
#include <iostream>
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
  ft_info dat;
#if 0 
  FT_Outline_Funcs funcs;
  funcs.move_to = (FT_Outline_MoveToFunc)&vector_glyph::move_to;
  funcs.line_to = (FT_Outline_LineToFunc)&vector_glyph::line_to;
  funcs.conic_to = (FT_Outline_ConicToFunc)&vector_glyph::conic_to;
  funcs.cubic_to = (FT_Outline_CubicToFunc)&vector_glyph::cubic_to;
  funcs.shift = 0;
  funcs.delta = 0;

  FT_Outline_Decompose( &outline, &funcs, &dat);
  error = FT_Outline_Decompose( &outline, &funcs, &dat);
  if(error)
    throw runtime_error("Fehler bei FT_Outline_Decompose");
#endif 
//if 0
  
  int start = 0, end = 0;
  for( std::size_t  i = 0; i < outline.n_contours; ++i )
  {
    start = end;
    end = outline.contours[i] + 1;


    for( int index = start; index != end; ++index )
    {
      dat.contours.push_back( std::pair<size_t,size_t>( dat.verts.size() - 1, 0 ) );
      char tag = outline.tags[index];

      if( tag == FT_Curve_Tag_On || (index + 1) == end )
      {
        dat.verts.push_back( v3_type(outline.points[index].x, outline.points[index].y, 0 ) );
      }
      else
      {
        v3_type control_point( outline.points[index].x, outline.points[index].y, 0 );
        v3_type previous = ( start == index )
          ? v3_type( outline.points[ end - 1].x , outline.points[ end - 1].y , 0 )
          : v3_type( outline.points[ index - 1].x , outline.points[ index - 1].y , 0 ); // hopfefully correct 
           // originally : dat.verts.back();
        v3_type next = ( index == end - 1 )
          ? dat.verts[dat.contours.back().first]
          : v3_type(outline.points[index+1].x, outline.points[index+1].y, 0 );

        if( tag == FT_Curve_Tag_Conic )
        {
          char next_tag = ( index == end - 1 )
            ? outline.tags[start]
            : outline.tags[index + 1];

          while( next_tag == FT_Curve_Tag_Conic )
          {
            next = ( control_point + next ) * 0.5;
            conic( dat.verts, previous, control_point, next );
            ++index;

            previous = next;
            control_point = v3_type(outline.points[index].x, outline.points[index].y, 0 );
            next = ( index == end - 1 )
              ? dat.verts[dat.contours.back().first]
              : v3_type(outline.points[index+1].x, outline.points[index+1].y, 0 );

            next_tag = ( index == end - 1 )
              ? outline.tags[start]
              : outline.tags[index + 1];
          }
          conic( dat.verts, previous, control_point, next );
        }
        else if( tag == FT_Curve_Tag_Cubic )
        {
          v3_type control_point_2 = next;
          next = ( index == end - 2 )
              ? dat.verts[dat.contours.back().first]
              : v3_type(outline.points[index+2].x, outline.points[index+2].y, 0 );

          cubic( dat.verts, previous, control_point, control_point_2, next );
          ++index;
        }
      }
    }

    dat.verts.push_back( v3_type( outline.points[  dat.contours.back().first ].x, outline.points[ dat.contours.back().first ].y, 0 ) );
    dat.contours.back().second = dat.verts.size() - dat.contours.back().first;
  }

  orig_data = dat.verts;

  GLUtesselator *tessel  = gluNewTess();
  gluTessProperty(tessel, GLU_TESS_WINDING_RULE, outline.flags & ft_outline_even_odd_fill ? GLU_TESS_WINDING_ODD : GLU_TESS_WINDING_NONZERO );
  gluTessCallback( tessel, GLU_TESS_BEGIN_DATA, reinterpret_cast<GLvoid (*)()>(&vector_glyph::begin) );
  gluTessCallback( tessel, GLU_TESS_VERTEX_DATA, reinterpret_cast<GLvoid (*)()>(&vector_glyph::vertex ));
  gluTessCallback( tessel, GLU_TESS_END_DATA, reinterpret_cast<GLvoid (*)()>(&vector_glyph::end ));
  gluTessCallback( tessel, GLU_TESS_COMBINE_DATA, reinterpret_cast<GLvoid (*)()>(&vector_glyph::combine ));
  gluTessCallback( tessel, GLU_TESS_ERROR_DATA, reinterpret_cast<GLvoid (*)()>(&vector_glyph::error ));

  gluTessProperty( tessel, GLU_TESS_TOLERANCE, 0);
  gluTessNormal( tessel, 0.0f, 0.0f, 1.0f);

  gluTessBeginPolygon( tessel, this );

  for( ft_info::contour_list::const_iterator it = dat.contours.begin(), e = dat.contours.end(); it != e; ++it )
  {
    gluTessBeginContour( tessel );
    for( std::size_t index = it->first, end = it->second + it->first; index != end ; ++index )
    {
      gluTessVertex(tessel, (double*)&(dat.verts[index]), (double*)&(dat.verts[index]));
    }
    gluTessEndContour(tessel);

  }

  gluTessEndPolygon(tessel);
  gluDeleteTess( tessel );
}

void vector_glyph::conic( std::vector<v3_type>& verts, v3_type const& begin, v3_type const& mid, v3_type const& end ) const
{
  for( unsigned int i = 1; i < 5; i++) 
  {
    v3_type bez1,bez2;

    double t = static_cast<double>(i) * 0.2;

    bez1 = (1.0 - t) * begin + t * mid;
    bez2 = (1.0 - t) * mid + t * end;
    bez1 = (1.0 - t) * bez1 + t * bez2;
    verts.push_back( bez1 );
  }       

  verts.push_back( end );
}

void vector_glyph::cubic( std::vector<v3_type>& verts, v3_type const& begin, v3_type const& mid_1, v3_type const& mid_2, v3_type const& end ) const
{
  for( unsigned int i = 1; i < 5; i++) 
  {
    v3_type bez1,bez2, bez3;

    double t = static_cast<double>(i) * 0.2;

    bez1 = (1.0 - t) * begin + t * mid_1;
    bez2 = (1.0 - t) * mid_1 + t * mid_2;
    bez3 = (1.0 - t) * mid_2 + t * end;
    bez1 = (1.0 - t) * bez1 + t * bez2;
    bez2 = (1.0 - t) * bez2 + t * bez3;
    bez1 = (1.0 - t) * bez1 + t * bez2;
    verts.push_back( bez1 );
  }       

  verts.push_back( end );
}

void vector_glyph::error( GLenum type, vector_glyph * p )
{
  std::cout << "glu :" << gluErrorString( type ) << std::endl;
}


void vector_glyph::begin( GLenum type, vector_glyph * p )
{
  p->ranges.push_back( vector_glyph::mesh_info( type, 0, p->va.size() ) );
}

void vector_glyph::end( vector_glyph * p )
{
  p->ranges.back().count = p->va.size() - p->ranges.back().first;
}
void vector_glyph::vertex( v3_type * data, vector_glyph * p )
{
  p->va.push_back( *data );
}

void vector_glyph::combine( double coords[3], void *d[4], GLfloat w[4], void **dataOut, vector_glyph * ptr ) 
{
  ptr->va.push_back( v3_type(coords[0],coords[1], coords[2] ) );
  *dataOut = &ptr->va.back();
}

void vector_glyph::render() const
{
  glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT );
  glEnableClientState( GL_VERTEX_ARRAY );
  glDisableClientState( GL_COLOR_ARRAY );
  glDisableClientState( GL_EDGE_FLAG_ARRAY );
  glDisableClientState( GL_INDEX_ARRAY );
  glDisableClientState( GL_NORMAL_ARRAY );
  glDisableClientState( GL_TEXTURE_COORD_ARRAY );
  glVertexPointer( 3, GL_DOUBLE, 0, &(orig_data[0]) );
  glPushMatrix();

  glScalef(1.0f/64.0f, 1.0f/64.0f, 1.0f/64.0f );
 // glDrawArrays( GL_LINE_LOOP, 0, orig_data.size() );
  glColor3f(0,0,1);
  glDrawArrays( GL_POINTS, 0, orig_data.size() );
  //glTranslatef(0,20,0);


  glVertexPointer( 3, GL_DOUBLE, 0, &(va[0]) );
  for( info_list::const_iterator it = ranges.begin(), e = ranges.end(); 
      it != e; ++it ) {
    glColor3f(0,1,0);
    glDrawArrays( GL_POINTS, it->first, it->count );
    glColor3f(1,0,0);
    glDrawArrays( GL_LINE_LOOP, it->first, it->count );
/*    glBegin( it->mode );
    for( std::size_t i = it->first, e = it->first + it->count; 
        i != e; ++i ) {
      std::cout << "glVertex3d(" << va[i][0] << ", " << va[i][1] << ", " << va[i][2] << ");" << std::endl;
      glVertex3dv( reinterpret_cast<double const*>(&(va[i])) );
    }
    glEnd();*/
  }
  glPopMatrix();
  glPopClientAttrib();
}

void vector_glyph::move_to( FT_Vector * to, ft_info * p )
{
  v3_type vec( to->x, to->y, 0 );
  p->contours.push_back( std::pair<size_t,size_t>( p->verts.size(), 1 ) );
  p->verts.push_back( vec );
}

void vector_glyph::line_to( FT_Vector * to, ft_info * p )
{
  v3_type vec( to->x, to->y, 0 );
  p->verts.push_back( vec );
  p->contours.back().second++;
}

void vector_glyph::conic_to( FT_Vector * control, FT_Vector * to, ft_info * p )
{
  v3_type begin = p->verts.back(), mid( control->x, control->y, 0), end( to->x, to->y, 0 );
  
  for( unsigned int i = 1; i < 5; i++) 
  {
    v3_type bez1,bez2;

    double t = static_cast<double>(i) * 0.2;

    bez1 = (1.0 - t) * begin + t * mid;
    bez2 = (1.0 - t) * mid + t * end;
    bez1 = (1.0 - t) * bez1 + t * bez2;
    p->verts.push_back( bez1 );
    p->contours.back().second++;
  }       

  p->verts.push_back( end );
  p->contours.back().second++;
}

void vector_glyph::cubic_to( FT_Vector * control1, FT_Vector * control2, FT_Vector * to, ft_info * p )
{
  v3_type begin = p->verts.back()
    , mid_1( control1->x, control1->y, 0)
    , mid_2( control2->x, control2->y, 0)
    , end( to->x, to->y, 0 );

  for( unsigned int i = 1; i < 5; i++) 
  {
    v3_type bez1,bez2, bez3;

    double t = static_cast<double>(i) * 0.2;

    bez1 = (1.0 - t) * begin + t * mid_1;
    bez2 = (1.0 - t) * mid_1 + t * mid_2;
    bez3 = (1.0 - t) * mid_2 + t * end;
    bez1 = (1.0 - t) * bez1 + t * bez2;
    bez2 = (1.0 - t) * bez2 + t * bez3;
    bez1 = (1.0 - t) * bez1 + t * bez2;
    p->verts.push_back( bez1 );
    p->contours.back().second++;
  }       

  p->verts.push_back( end );
  p->contours.back().second++;

}


vector_glyph::mesh_info::mesh_info( GLenum m_, std::size_t c_, std::size_t f_ ) 
: mode(m_), count(c_), first(f_) 
{
}


}}}

