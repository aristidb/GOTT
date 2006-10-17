/***************************************************************************
 *   Copyright (C) 2004 by Andreas Pokorny <andreas.pokorny@gmx.de>        *
 *                                                                         *
 * This file is part of the Gott gui toolkit.                              *
 *                                                                         *
 * Gott is free software; you can redistribute it and/or modify            *
 * it under the terms of the GNU Lesser General Public License as          *
 * published by the Free Software Foundation; either version 2 of the      *
 * License, or (at your option) any later version.                         *
 *                                                                         *
 * Gott is distributed in the hope that it will be useful,                 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this software; if not, write to the Free Software            *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.               *
 ***************************************************************************/

#ifndef GOTT_UUID_H_INCLUDED
#define GOTT_UUID_H_INCLUDED

#include <boost/cstdint.hpp>
#include <iostream>

namespace gott 
{
struct uuid
{
	public:
    boost::uint32_t time_low;
    boost::uint16_t time_mid;
    boost::uint16_t time_hi_and_version;
    boost::uint16_t clock_seq;
    boost::uint8_t node[6];
		void generate();
		void to_buffer( unsigned char* buffer) const;
	private:
		void generate_random();
		void generate_time();
		void read_from_buffer( unsigned char const* buffer);
		void write_to_buffer( unsigned char * buffer) const;
		
};

void read_from_string( std::string const& data, gott::uuid & obj );
void write_to_string( std::string & data, gott::uuid const& obj );


}

std::ostream& operator<<( std::ostream & out, gott::uuid const& obj );
std::istream& operator>>( std::istream & in, gott::uuid & obj );
bool operator==( gott::uuid const& left, gott::uuid const& right);
bool operator!=( gott::uuid const& left, gott::uuid const& right);

#endif

