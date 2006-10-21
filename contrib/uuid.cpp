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

/* Big parts of the code are based on the gen_uuid.c file in the efs2progs*/

/*
 * gen_uuid.c --- generate a DCE-compatible uuid
 *
 * Copyright (C) 1996, 1997, 1998, 1999 Theodore Ts'o.
 *
 * %Begin-Header%
 * This file may be redistributed under the terms of the GNU 
 * Library General Public License.
 * %End-Header%
 */

/*
 * Force inclusion of SVID stuff since we need it if we're compiling in
 * gcc-wall wall mode
 */

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <cstdlib>
#include <stdexcept>
#include <cstring>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/file.h>
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_SYS_SOCKIO_H
#include <sys/sockio.h>
#endif
#ifdef HAVE_NET_IF_H
#include <net/if.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#include <algorithm>

#include <boost/spirit/core/primitives/numerics.hpp>
#include <boost/spirit/core/parser.hpp>
#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/spirit/core/composite/actions.hpp>
#include <boost/spirit/core/composite/sequence.hpp>
#include <boost/spirit/core/composite/operators.hpp>
//#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>

#include "uuid.h"



#ifdef HAVE_SRANDOM
#define srand(x) 	srandom(x)
#define rand() 		random()
#endif

namespace {
int get_random_fd(void)
{
	struct timeval	tv;
	static int	fd = -2;
	int		i;

	if (fd == -2) {
		gettimeofday(&tv, 0);
		fd = open("/dev/urandom", O_RDONLY);
		if (fd == -1)
			fd = open("/dev/random", O_RDONLY | O_NONBLOCK);
		srand((getpid() << 16) ^ getuid() ^ tv.tv_sec ^ tv.tv_usec);
	}
	/* Crank the random number generator a few times */
	gettimeofday(&tv, 0);
	for (i = (tv.tv_sec ^ tv.tv_usec) & 0x1F; i > 0; i--)
		rand();
	return fd;
}


/*
 * Generate a series of random bytes.  Use /dev/urandom if possible,
 * and if not, use srandom/random.
 */
void get_random_bytes( unsigned char* buf, int nbytes)
{
	int i, n = nbytes, fd = get_random_fd();
	int lose_counter = 0;
	unsigned char *cp = buf;

	if (fd >= 0) {
		while (n > 0) {
			i = read(fd, cp, n);
			if (i <= 0) {
				if (lose_counter++ > 16)
					break;
				continue;
			}
			n -= i;
			cp += i;
			lose_counter = 0;
		}
	}
	
	/*
	 * We do this all the time, but this is the only source of
	 * randomness if /dev/random/urandom is out to lunch.
	 */
	for (cp = buf, i = 0; i < nbytes; i++)
		*cp++ ^= (rand() >> 7) & 0xFF;
	return;
}

/*
 * Get the ethernet hardware address, if we can find it...
 */
int get_node_id(unsigned char *node_id)
{
#ifdef HAVE_NET_IF_H
	int 		sd;
	ifreq 	ifr, *ifrp;
	ifconf 	ifc;
	char buf[1024];
	int		n, i;
	unsigned char 	*a;
	
/*
 * BSD 4.4 defines the size of an ifreq to be
 * max(sizeof(ifreq), sizeof(ifreq.ifr_name)+ifreq.ifr_addr.sa_len
 * However, under earlier systems, sa_len isn't present, so the size is 
 * just sizeof(struct ifreq)
 */
#ifdef HAVE_SA_LEN
#define ifreq_size(i) std::max(sizeof(struct ifreq),\
     sizeof((i).ifr_name)+(i).ifr_addr.sa_len)
#else
#define ifreq_size(i) sizeof(struct ifreq)
#endif /* HAVE_SA_LEN*/

	sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (sd < 0) {
		return -1;
	}
	memset(buf, 0, sizeof(buf));
	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = buf;
	if (ioctl (sd, SIOCGIFCONF, (char *)&ifc) < 0) {
		close(sd);
		return -1;
	}
	n = ifc.ifc_len;
	for (i = 0; i < n; i+= ifreq_size(*ifr) ) {
		ifrp = (struct ifreq *)((char *) ifc.ifc_buf+i);
		strncpy(ifr.ifr_name, ifrp->ifr_name, IFNAMSIZ);
#ifdef SIOCGIFHWADDR
		if (ioctl(sd, SIOCGIFHWADDR, &ifr) < 0)
			continue;
		a = (unsigned char *) &ifr.ifr_hwaddr.sa_data;
#else
#ifdef SIOCGENADDR
		if (ioctl(sd, SIOCGENADDR, &ifr) < 0)
			continue;
		a = (unsigned char *) ifr.ifr_enaddr;
#else
		/*
		 * XXX we don't have a way of getting the hardware
		 * address
		 */
		close(sd);
		return 0;
#endif /* SIOCGENADDR */
#endif /* SIOCGIFHWADDR */
		if (!a[0] && !a[1] && !a[2] && !a[3] && !a[4] && !a[5])
			continue;
		if (node_id) {
			memcpy(node_id, a, 6);
			close(sd);
			return 1;
		}
	}
	close(sd);
#endif
	return 0;
}

/* Assume that the gettimeofday() has microsecond granularity */
#define MAX_ADJUSTMENT 10

int get_clock(boost::uint32_t &clock_high, boost::uint32_t &clock_low, boost::uint16_t &ret_clock_seq)
{
	static int			adjustment = 0;
	static struct timeval		last = {0, 0};
	static boost::uint16_t			clock_seq;
	struct timeval 			tv;
	unsigned long long		clock_reg;
	
try_again:
	gettimeofday(&tv, 0);
	if ((last.tv_sec == 0) && (last.tv_usec == 0)) {
		get_random_bytes(reinterpret_cast<unsigned char*>(&clock_seq), sizeof(clock_seq));
		clock_seq &= 0x1FFF;
		last = tv;
		last.tv_sec--;
	}
	if ((tv.tv_sec < last.tv_sec) ||
	    ((tv.tv_sec == last.tv_sec) &&
	     (tv.tv_usec < last.tv_usec))) {
		clock_seq = (clock_seq+1) & 0x1FFF;
		adjustment = 0;
		last = tv;
	} else if ((tv.tv_sec == last.tv_sec) &&
	    (tv.tv_usec == last.tv_usec)) {
		if (adjustment >= MAX_ADJUSTMENT)
			goto try_again;
		adjustment++;
	} else {
		adjustment = 0;
		last = tv;
	}
		
	clock_reg = tv.tv_usec*10 + adjustment;
	clock_reg += ((unsigned long long) tv.tv_sec)*10000000;
	clock_reg += (((unsigned long long) 0x01B21DD2) << 32) + 0x13814000;

	clock_high = clock_reg >> 32;
	clock_low = clock_reg;
	ret_clock_seq = clock_seq;
	return 0;
}

template <typename T>
inline unsigned char const* read_bytes( unsigned char const* data, T & value )
{
	value = * data;
	for( int i = 1; i < sizeof(T); ++i )
		value = (value << 8) | *data++;
	return data;
}

template <typename T>
inline unsigned char* write_bytes( unsigned char* data, T const& value )
{
	T cp = value;
	for( int i = sizeof(T) - 1; i>-1; --i )
	{
		*(data + i) = boost::uint8_t( cp );
		cp >>= 8;
	}
	return data+sizeof(T);
}
}

void gott::uuid::read_from_buffer( unsigned char const* buffer)
{
	unsigned char const*buf = read_bytes(
			read_bytes(
				read_bytes(
					read_bytes(buffer, time_low),
					time_mid), 
				time_hi_and_version),
			clock_seq 
			);
	std::memcpy( node, buf, 6 );
}

void gott::uuid::write_to_buffer( unsigned char * buffer) const
{
	unsigned char * buf = write_bytes(
			write_bytes(
				write_bytes(
					write_bytes(buffer, time_low),
					time_mid),
				time_hi_and_version),
			clock_seq);
	std::memcpy( buf, node, 6 );
}

void gott::uuid::generate_time()
{
	static unsigned char node_id[6];
	static int has_init = 0;
	boost::uint32_t	clock_mid;

	if (!has_init) {
		if (get_node_id( node_id ) <= 0) {
			get_random_bytes( node_id, 6);
			/*
			 * Set multicast bit, to prevent conflicts
			 * with IEEE 802 addresses obtained from
			 * network cards
			 */
			node_id[0] |= 0x80;
		}
		has_init = 1;
	}
	get_clock( clock_mid, time_low, clock_seq);
	clock_seq |= 0x8000;
	time_mid = boost::uint16_t (clock_mid) ;
	time_hi_and_version = (clock_mid >> 16) | 0x1000;
	std::memcpy( node, node_id, 6 );
}

void gott::uuid::generate_random()
{
	unsigned char buf[16];

	get_random_bytes( buf, sizeof(buf) );
	read_from_buffer( buf );

	clock_seq = (clock_seq & 0x3FFF) | 0x8000;
	time_hi_and_version = (time_hi_and_version & 0x0FFF) | 0x4000;
}

/*
 * This is the generic front-end to uuid_generate_random and
 * uuid_generate_time.  It uses uuid_generate_random only if
 * /dev/urandom is available, since otherwise we won't have
 * high-quality randomness.
 */
void gott::uuid::generate()
{
	if (get_random_fd() >= 0)
		generate_random();
	else
		generate_time();
}

void gott::uuid::to_buffer( unsigned char* buffer ) const
{
	write_to_buffer( buffer );
}

namespace gott
{
void read_from_string( const std::string & source, gott::uuid& id  )
{
	using namespace boost::spirit;
	// write that parser!
	uint_parser<boost::uint8_t, 16, 2, 2> hex_1;
	uint_parser<boost::uint16_t, 16, 4, 4> hex_2;
	uint_parser<boost::uint32_t, 16, 8, 8> hex_4;
	
	parse_info<const char *> p_info = parse(
			source.c_str(),
			source.c_str() + source.length(),
			hex_4[ assign_a( id.time_low ) ] >>	'-' >>
			hex_2[ assign_a( id.time_mid ) ] >> '-' >>           
			hex_2[ assign_a( id.time_hi_and_version)] >> '-' >>           
			hex_2[ assign_a( id.clock_seq)] >> '-' >>
			hex_1[ assign_a(id.node[0])] >>
			hex_1[ assign_a(id.node[1])] >>
			hex_1[ assign_a(id.node[2])] >>
			hex_1[ assign_a(id.node[3])] >>
			hex_1[ assign_a(id.node[4])] >>
			hex_1[ assign_a(id.node[5])]
			);

	if( !p_info.hit || p_info.length != 36 )
		throw std::runtime_error( ("Error while parsing uuid: " + source).c_str() );
}
void write_to_string( std::string & output, const gott::uuid& id  )
{
	char out[37];
	unsigned char buffer[16];
	id.to_buffer( buffer );
	char converter[]="0123456789ABCDEF";
	size_t out_index = 0;
	for( size_t i = 0; i < 16 ; ++i )
	{
		out[ out_index++ ] = converter[buffer[i]>>4];
		out[ out_index++ ] = converter[buffer[i]%16];
		if( i == 3 || i == 5 || i == 7 || i == 9 )
				out[ out_index++ ] = '-';
	}
	out[36] = '\0';

	output = out;
}

}

std::ostream& operator<<( std::ostream & out, gott::uuid const& obj )
{
	std::string temp;
	gott::write_to_string( temp, obj );
	out << temp;
	return out; 
}

std::istream& operator>>( std::istream & in, gott::uuid & obj )
{
	std::string temp;
	in >> temp;
	try {
	gott::read_from_string( temp, obj );
	} catch (std::runtime_error & e)  {
		for( std::string::const_reverse_iterator it = temp.rbegin(), e = temp.rend();
				it != e; ++it )
			in.putback(*it);
		in.setstate(std::ios::failbit);
	}
	
	return in;
}

bool operator==( gott::uuid const& left, gott::uuid const& right)
{
	return 
		( left.time_low == right.time_low ) &&
		( left.time_mid == right.time_mid ) &&
		( left.time_hi_and_version == right.time_hi_and_version) &&
		( left.clock_seq == right.clock_seq ) &&
		( left.node[0] == right.node[0] ) &&
		( left.node[1] == right.node[1] ) &&
		( left.node[2] == right.node[2] ) &&
		( left.node[3] == right.node[3] ) &&
		( left.node[4] == right.node[4] ) &&
		( left.node[5] == right.node[5] );
}
bool operator!=( gott::uuid const& left, gott::uuid const& right)
{
	return !(left == right);
}



#ifdef UUIDGEN_TEST
int main()
{
	gott::uuid test;
	test.generate();
	std::cout << test  << std::endl;
	std::cout << "Writing uuid to string to test parsing:\"";
	std::string content;
	gott::write_to_string( content, test );
	std::cout << content << "\"" << std::endl;
	
	gott::uuid other_uuid;
	gott::read_from_string( content, other_uuid );
	std::cout << "Reparsed uuid:                          " << other_uuid << std::endl;
	std::cout << "EEEK I am not cheating" << std::endl;
	std::cout << "Testing equality: generated == parsed ? " <<  (test == other_uuid) << std::endl;
	gott::uuid uuid_again;
	uuid_again.generate();
	std::cout << "Testing equality: generated == 2nd_generated ? " <<  (test == uuid_again) << std::endl;
	
}
#endif
