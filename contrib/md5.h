/* md5.h - Declaration of functions and data types used for MD5 sum
   computing library functions.
   Copyright (C) 1995, 1996, 1999, 2000, 2003 Free Software Foundation, Inc.
   NOTE: The canonical source of this file is maintained with the GNU C
   Library.  Bugs can be reported to bug-glibc@prep.ai.mit.edu.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef GOTT_MD5_H_INCLUDED
#define GOTT_MD5_H_INCLUDED

#include <string>
#include <iostream>
#include <boost/cstdint.hpp>

namespace gott {
/* Structure to save state of computation between the single steps.  */

class MD5
{
	public:
		struct Sum // TODO
		{
			unsigned char data[16];
		};
	private:
    boost::uint32_t A;
		boost::uint32_t B;
		boost::uint32_t C;
		boost::uint32_t D;

		boost::uint32_t total[2];
		boost::uint32_t buflen;
		char internal_buffer[128];
		void read_ctx( Sum& result );
		void finish_ctx( Sum& result );
		void process_block( char const* words, std::size_t char_len );
		void process_bytes( char const* buffer, std::size_t char_len );
		void init();
	public:
		/** Initialize structure containing state of computation.
		   (RFC 1321, 3.3: Step 3)  */
		bool file( const char *filename, Sum& result );
		void buffer( const char* buffer, std::size_t char_len, Sum& result );
		
};

void read_from_string( std::string const& source, gott::MD5::Sum & obj );
void write_to_string( std::string & data, gott::MD5::Sum const& obj );

}


bool operator==( gott::MD5::Sum const& left, gott::MD5::Sum const & right );
bool operator!=( gott::MD5::Sum const& left, gott::MD5::Sum const & right ); 
std::istream& operator>>( std::istream& input, gott::MD5::Sum & object );
std::ostream& operator<<( std::ostream& input, gott::MD5::Sum const& object );

#endif
