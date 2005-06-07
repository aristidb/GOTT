
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <GL/glu.h>
#include <freetype/ftoutln.h>
#include "vector_glyph.hpp"
#include "../math/hyper_plane.hpp"

using namespace std;

namespace gott{namespace gui{namespace font{

struct line {
  vector_glyph::contour_point *a, *b;
  gott::math::hyper_plane2<float> border; 
  bool is_contour; 
  line( vector_glyph::contour_point *first, vector_glyph::contour_point *second, vector_glyph::contour_point *test_point, bool is_contour = false );
  /**
   *
   */
  vector_glyph::contour_point * get_neighbour_point_of_a() const;
  vector_glyph::contour_point * get_neighbour_point_of_b() const;
};

line::line(vector_glyph::contour_point *f, vector_glyph::contour_point *s, vector_glyph::contour_point *t, bool is_c )
  : a(f), b(s), border(f->point, s->point, t->point ), is_contour(is_c)
{
}

vector_glyph::contour_point * line::get_neighbour_point_of_a() const
{
  return ( a->next ==  b )?a->previous:a->next;
}
vector_glyph::contour_point * line::get_neighbour_point_of_b() const
{
  return ( b->next ==  a )?b->previous:b->next;
}


vector_glyph::contour_point::contour_point( float x, float y, contour_point * prev, contour_point* ne )
  : point(x,y), previous(prev), next(ne), array_index(-1)
{
  if( next )
    next->previous = this;
  if( previous)
    previous->next= this;
}


vector_glyph::contour_point::contour_point( vector_glyph::v2_type const &p, contour_point * prev, contour_point* ne )
  : point(p), previous(prev), next(ne), array_index(-1)
{
  if( next )
    next->previous = this;
  if( previous)
    previous->next= this;
}

vector_glyph::vector_glyph( FT_Face & face, std::size_t glyph_index )
{
  int error = FT_Load_Glyph( face, glyph_index, FT_LOAD_NO_BITMAP );
  if(error)
    throw runtime_error("Fehler bei FT_Set_Char_Size");

  FT_Outline & outline =  face->glyph->outline;
  

  /*
   * The following for loop creates the final contours from the contours found 
   * in the freetype outline structure. conic and cubic parts get transfered into
   * discrete line segments. 
   */
  int start = 0, end = 0;
  for( int  i = 0; i < outline.n_contours; ++i )
  {
    start = end;
    end = outline.contours[i] + 1;


    int my_end = end;
    contour_point *first_point = 0, *last_point = 0;
    for( int index = start; index < my_end; ++index )
    {
      char tag = outline.tags[index];

      //if( tag == FT_Curve_Tag_On || (index + 1) == end )
      //if( tag & 1 || (index + 1) == end )
      //{
        if( first_point == 0 ) {
          first_point = new contour_point( outline.points[index].x, outline.points[index].y  );
          last_point = first_point->next = first_point->previous = first_point;
          points.push_back( first_point );
        }
        else {
          points.push_back( new contour_point( outline.points[index].x, outline.points[index].y, last_point, first_point ) ); 
          last_point->next = points.back();
          last_point = points.back();
        }/*
      }
      else
      {
        if( first_point == 0 ) // assume that the last point in contour is an in point ... hopefully correct!
        {
          first_point = new contour_point( outline.points[my_end - 1].x, outline.points[my_end -1].y  );
          last_point = first_point->next = first_point->previous = first_point;
          points.push_back( first_point );
          --my_end; // reduce my_end
        }
        
        v2_type control_point( outline.points[index].x, outline.points[index].y );
        contour_point * previous = points.back();

        if( ! tag & 2 )
        {
          char next_tag = ( index == my_end - 1 )
            ? outline.tags[start]
            : outline.tags[index + 1];

          //while( next_tag == FT_Curve_Tag_Conic )
          contour_point * next;

          v2_type data;
          if( index == my_end - 1 )  data = first_point->point;
          else data = v2_type(outline.points[index+1].x, outline.points[index+1].y);

          if( next_tag & 2 ) // further conic segments follow => median of two control points defines contour point
            data =  ( data + control_point ) * 0.5f;
          
          if( index == my_end - 1 ){
            first_point->point = data;
            next = first_point;
          }
          else  {
            next = new contour_point( data, 0, first_point ); 
            points.push_back( next );
            last_point = next;
          }

          conic( points, previous, control_point, next );
        }
        else if( tag & 2 )
        {
          contour_point *next;
          v2_type control_point_2;
          if( index == my_end - 1 ){
            control_point_2  = first_point->point;
            next = first_point->next;
          }
          else if( index == my_end -2 ) {
            control_point_2 = v2_type ( outline.points[index+1].x, outline.points[index+1].y );
            next = first_point;
          }
          else {
            control_point_2 = v2_type ( outline.points[index+1].x, outline.points[index+1].y );
            next = new contour_point(outline.points[index+2].x, outline.points[index+2].y, 0, first_point );
            points.push_back( next );
            last_point = next;
          }

          cubic( points, previous, control_point, control_point_2, next );
          ++index;
        }
      }*/
    }
    //  dat.verts.push_back( v2_type( outline.points[  dat.contours.back().first ].x, outline.points[ dat.contours.back().first ].y  ) );
  
    contours.push_back( first_point );
  }


  // All points of ALL contours get reorderd by the x-coordinate 
  points.sort( scanline_ordering() );


#if 0 

  typedef std::list<line> open_lines_type;
  open_lines_type open_lines;
  typedef std::list<contour_point*> open_points_type;
  open_points_type open_points;

  vertex_array.reserve( points.size() );

  for( std::list<contour_point*>::const_iterator it = points.begin(), e = points.end(); 
      it != e; ++ it ) {
    /* This Slope implements a scanline algorithm  
    
     First problem knowing where in and out is: 
     
     y /|\ 
        |  1 3    7 11
        |     
        |    4    8   
        |     
        |    5    9   
        |        
        |  2 6   10 12
        |
        +-------------------> x
        a possible ordering of vertices when tesselateting the glyph H
        the vertical ordering is abitrary!
    
     The first contour parts / lines / adjacent points, that will be found in points 
     mark the left border(s) of a glyph. Thus the right side of the line is 'in' the 
     glyph. Contour lines and inner lines of the glyph are called open lines, lines which
     need triangles to reach the other 'side' of a contour, or another contour to end the
     'in' region 
    
        Point_x                                Point_x     point_x+m 
          *_________    some time later we       *__________* 
       out *__in____    might find  =>        out *__in____*  'out'
            *_______                               *______*
           Point_x+n                              Point_x+n  
    
     The Line between Point_x+m and Point_x+n closes the open line between point_x and point_x+n. 
     It is possible to calculate the normal and projection value of a hyper plane which will then 
     describe the in and out region of that open line. 
    
     The list open_lines keeps all 'open' lines. 
     Open points stores all points which could not yet be used to construct a triangle or opening and 
     closeing line. These points need other points to create lines.
    
     Second Problem: Creating Triangles:
      This is handled in the open_line-for_loop. To find possible triangles, the algorithm searches for 
      possible base lines, in the open_line list. For a given point a possible base line of a triangle is
      a line whith a hyper_plane stateing the given point to be on the "in" side.
      Q: Is that enough, to gurantee corectness? 
         
    
     some uncomplete outline of the algorithm:
     we need to first integrate a new point
     for all open_lines
      1. Get all lines with positive distance greater than a epsilon to new point 
        and more ifs 
    
     handle lines found:
      reorder lines
       lines with contour neighbours of new points get higher priority.
       Idea: to make ugly triangles less likely:
        * belohne adjacent pairs of open lines
        * punish big projections on open line / distance to open line
      create triangles from the first two lines or more
       for each triangle to be created
        remove line from open_lines 
        if neighbour one is no contour
         add new_point n_one to open_lines
        if neighbour two is no contour
         add new_point n_two to open_lines
        addtrianges to storage
       
    
     if none found 
      for all open points
        if new point is neighbour 
          erase current point 
          add a new line to open_lines
    
     if none found add point to open_point
     */
   
       bool point_integrated = false;

     for( open_points_type::iterator p_it = open_points.begin(), p_e = open_points.end();
         p_it != p_e;  ){
       if( *it == (*p_it)->next ){ // which next and previous is looked at is decisive!! <- derives from the fact that the points are looked through ordered 
         open_lines.push_back( line( *it, *p_it, (*p_it)->previous , true ) );
         p_it = open_points.erase( p_it );
         point_integrated = true;
       }
       else if( *it == (*p_it)->previous ){
         open_lines.push_back( line( *it, *p_it, (*p_it)->next , true) );
         p_it = open_points.erase( p_it );
         point_integrated = true;
       }
       else 
       {
         //  std::cout << "*it:" << *it << " (*p_it)->previous:" << (*p_it)->previous << " (*p_it)->next" << (*p_it)->next << std::endl;
         ++p_it;
       }
     }
     std::list<open_lines_type::iterator> possible_lines;

     for( std::list<line>::iterator l_it = open_lines.begin(), l_e = open_lines.end(); 
         l_it != l_e; ++l_it ){
       if( l_it->a != *it && l_it->b != *it ) {
         if( l_it->border.distance( (*it)->point ) > 0.01f ){
           possible_lines.push_front( l_it ); 
           if( (*it)->next == l_it->a || (*it)->next == l_it->b ){
             possible_lines.push_front( l_it ); 
           }
           else { 
             contour_point * n_a = l_it->get_neighbour_point_of_a();
             contour_point * n_b = l_it->get_neighbour_point_of_b();
             math::hyper_plane2<float> p_a, p_b;
             if( n_a )
               p_a = math::hyper_plane2<float>( l_it->a->point, n_a->point, l_it->b->point ); 
             if( n_b )
               p_b = math::hyper_plane2<float>( l_it->b->point, n_b->point, l_it->a->point ); 

             if( n_a && n_b && p_a.distance( (*it)->point ) > 0 &&  p_b.distance( (*it)->point ) > 0 )  // this test suffices because points are ordered
               possible_lines.push_back( l_it );
           }
        }
      }
    }


   

    if( ! point_integrated &&  possible_lines.empty() ) 
      open_points.push_back( *it );

    for( std::list<open_lines_type::iterator>::const_iterator l_it = possible_lines.begin(), l_e = possible_lines.end();
        l_it != l_e; ++l_it ) {
      //(*it)->array_index = vertex_array.size();
      vertex_array.push_back( (*it)->point );

      //if( l_it->a->array_index == -1 ) {
      // l_it->a->array_index = vertex_array.size();
      vertex_array.push_back( (*l_it)->a->point);
      //        }
      //       if( l_it->b->array_index == -1 ) {
      //       l_it->b->array_index = vertex_array.size();
      vertex_array.push_back( (*l_it)->b->point);
      //   }

      /* index_array.push_back( l_it->a->array_index );
         index_array.push_back( l_it->b->array_index );
         index_array.push_back( (*it)->array_index );*/
      open_lines.erase( *l_it );
      if( !( (*it)->next == (*l_it)->a || (*it)->previous == (*l_it)->a) )
      {
        open_lines.push_back( line( (*it), (*l_it)->a, (*l_it)->b, false) );
      }
      if( !( (*it)->next == (*l_it)->b || (*it)->previous == (*l_it)->b) )
        open_lines.push_back( line( (*it), (*l_it)->b, (*l_it)->a, false ) );
    }
  } 
#endif

  /*  while( ! points.empty() )
      {
      delete points.back();
      points.pop_back();
      }*/
}

void vector_glyph::conic( std::list<contour_point*>& points, contour_point * begin, v2_type const& mid, contour_point * end ) const
{
  contour_point *previous = begin;
  for( unsigned int i = 1; i < 5; i++) 
  {
    v2_type bez1,bez2;

    float t = float(i) * 0.2;

    bez1 = (1.0f - t) * begin->point + t * mid;
    bez2 = (1.0f - t) * mid + t * end->point;
    bez1 = (1.0f - t) * bez1 + t * bez2;

    previous = new contour_point( bez1, previous, 0 );
    points.push_back( previous );
  }
  end->previous = previous;
  previous->next = end;

}

void vector_glyph::cubic( std::list<contour_point*>& points, contour_point * begin, v2_type const& mid_1, v2_type const& mid_2, contour_point * end ) const
{
  contour_point *previous = begin;
  for( unsigned int i = 1; i < 5; i++) 
  {
    v2_type bez1,bez2, bez3;

    float t = float(i) * 0.2f;

    bez1 = (1.0f - t) * begin->point + t * mid_1;
    bez2 = (1.0f - t) * mid_1 + t * mid_2;
    bez3 = (1.0f - t) * mid_2 + t * end->point;
    bez1 = (1.0f - t) * bez1 + t * bez2;
    bez2 = (1.0f - t) * bez2 + t * bez3;
    bez1 = (1.0f - t) * bez1 + t * bez2;

    previous = new contour_point( bez1, previous, 0 );
    points.push_back( previous );
  }       

  end->previous = previous;
  previous->next = end;
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


  glColor4f(1,0,1,0.5);
  for(std::list<contour_point*>::const_iterator it = contours.begin(), e = contours.end(); it != e; ++it )
  {
    glBegin( GL_LINE_LOOP );
    contour_point * p = *it;
    do {
      std::cout << p << std::endl;
      glVertex2fv(reinterpret_cast<float*>(&(p->point)));
      p=p->next;
    }
    while( *it != p );
    glEnd();
  }

  for( size_t i = 0; i < vertex_array.size(); i += 3 )
  {
    glBegin( GL_TRIANGLES );
    glColor4f(1.0,0.0,0.0,0.25);glVertex2fv( reinterpret_cast<float const*>(&vertex_array[i]));
    glColor4f(0.0,1.0,0.0,0.25);glVertex2fv( reinterpret_cast<float const*>(&vertex_array[i+1]));
    glColor4f(0.0,0.0,1.0,0.25);glVertex2fv( reinterpret_cast<float const*>(&vertex_array[i+2]));
    glEnd();
  }
//  glDrawArrays( GL_TRIANGLES, 0, vertex_array.size() );

  glPopMatrix();
  glPopClientAttrib();

}



}}}

