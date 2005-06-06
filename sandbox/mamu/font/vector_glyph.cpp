
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
  std::cout << "Constructed with " << this << ", " << previous << ", " << next << std::endl;
}


vector_glyph::contour_point::contour_point( vector_glyph::v2_type const &p, contour_point * prev, contour_point* ne )
  : point(p), previous(prev), next(ne), array_index(-1)
{
  if( next )
    next->previous = this;
  if( previous)
    previous->next= this;
  std::cout << "Constructed with " << this << ", " << previous << ", " << next << std::endl;
}

vector_glyph::vector_glyph( FT_Face & face, std::size_t glyph_index )
{
  int error = FT_Load_Glyph( face, glyph_index, FT_LOAD_NO_BITMAP );
  if(error)
    throw runtime_error("Fehler bei FT_Set_Char_Size");

  FT_Outline & outline =  face->glyph->outline;
  

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
/*      if( tag & 1 || (index + 1) == end )
      {*/
        if( first_point == 0 ) {
          first_point = new contour_point( outline.points[index].x, outline.points[index].y  );
          last_point = first_point->next = first_point->previous = first_point;
          points.push_back( first_point );
        }
        else {
          contour_point * n_p = new contour_point( outline.points[index].x, outline.points[index].y ); 
          n_p->previous = last_point;
          n_p->next= first_point;

          last_point->next = n_p;
          first_point->previous=n_p;

          last_point = n_p;

          points.push_back( n_p );
          //      points.back()->next = first_point;
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

  points.sort( scanline_ordering() );



  typedef std::list<line> open_lines_type;
  open_lines_type open_lines;
  typedef std::list<contour_point*> open_points_type;
  open_points_type open_points;

  vertex_array.reserve( points.size() );
 // index_array.reserve( points.size() );
  
  for( std::list<contour_point*>::const_iterator it = points.begin(), e = points.end(); 
      it != e; ++ it ) {
    // we need to first integrate a new point
    // for all open_lines
    //  1. Get all lines with positive distance greater than a epsilon to new point 
    //    and more ifs 
    //
    // handle lines found:
    //  reorder lines
    //   lines with contour neighbours of new points get higher priority.
    //   Idea: to make ugly triangles less likely:
    //    * belohne adjacent pairs of open lines
    //    * punish big projections on open line / distance to open line
    //  create triangles from the first two lines or more
    //   for each triangle to be created
    //    remove line from open_lines 
    //    if neighbour one is no contour
    //     add new_point n_one to open_lines
    //    if neighbour two is no contour
    //     add new_point n_two to open_lines
    //    addtrianges to storage
    //   
    //
    // if none found 
    //  for all open points
    //    if new point is neighbour 
    //      erase current point 
    //      add a new line to open_lines
    //
    // if none found add point to open_point
    
    std::list<open_lines_type::iterator> possible_lines;

    for( std::list<line>::iterator l_it = open_lines.begin(), l_e = open_lines.end(); 
        l_it != l_e; ++l_it ){
      
      if( l_it->border.distance( (*it)->point ) > 0.01f ){
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


    if( possible_lines.empty() )  {
      bool something_found = false;

      for( open_points_type::iterator p_it = open_points.begin(), p_e = open_points.end();
          p_it != p_e;  ){
        if( *it == (*p_it)->next ){ // which next and previous is looked at is decisive!! <- derives from the fact that the points are looked through ordered 
          open_lines.push_back( line( *it, *p_it, (*p_it)->previous , true ) );
          p_it = open_points.erase( p_it );
          something_found = true;
        }
        else if( *it == (*p_it)->previous ){
          open_lines.push_back( line( *it, *p_it, (*p_it)->next , true) );
          p_it = open_points.erase( p_it );
          something_found = true;
        }
        else 
        {
        //  std::cout << "*it:" << *it << " (*p_it)->previous:" << (*p_it)->previous << " (*p_it)->next" << (*p_it)->next << std::endl;
          ++p_it;
        }
      }

      if( ! something_found )
        open_points.push_back( *it );
    }
    else
    {
      
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
  } 
    
  

  while( ! points.empty() )
  {
    delete points.back();
    points.pop_back();
  }
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


  glColor4f(1,0,1,0.125);
  
  glDrawArrays( GL_TRIANGLES, 0, vertex_array.size() );

  glPopMatrix();
  glPopClientAttrib();
  
}



}}}

