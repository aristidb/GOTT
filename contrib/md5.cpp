/* md5.c - Functions to compute MD5 message digest of files or memory blocks
   according to the definition of MD5 in RFC 1321 from April 1992.
   Copyright (C) 1995, 1996, 2001, 2003 Free Software Foundation, Inc.
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

/* Written by Ulrich Drepper <drepper@gnu.ai.mit.edu>, 1995.  */

// Converted to C++ by Andreas Pokorny <andreas.pokorny@gmx.de>
// extended by boost parsers, and std-io routines 

#define rol(x,n) ( ((x) << (n)) | ((x) >> (32-(n))) )

#include "md5.h"

#ifdef HAVE_CONFIG_H
#include "../../config.h"
#endif

#ifdef SYS_HAVE_TYPES_H
#include <sys/types.h>
#endif 

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <boost/spirit/core/primitives/numerics.hpp>
#include <boost/spirit/core/parser.hpp>
#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/spirit/core/composite/actions.hpp>
#include <boost/spirit/core/composite/sequence.hpp>
#include <boost/spirit/core/composite/operators.hpp>


#ifdef WORDS_BIGENDIAN
# define SWAP(n)							\
	(((n) << 24) | (((n) & 0xff00) << 8) | (((n) >> 8) & 0xff00) | ((n) >> 24))
#else
# define SWAP(n) (n)
#endif

#define BLOCKSIZE 4096
/* Ensure that BLOCKSIZE is a multiple of 64.  */
#if BLOCKSIZE % 64 != 0
#error "invalid BLOCKSIZE"
#endif

/* This array contains the bytes used to pad the buffer to the next
   64-byte boundary.  (RFC 1321, 3.1: Step 1)  */
static const unsigned char fillbuf[64] = { 0x80, 0 /* , 0, 0, ...  */ };


/* Initialize structure containing state of computation.
   (RFC 1321, 3.3: Step 3)  */
void gott::MD5::init()
{
	A = 0x67452301;
	B = 0xefcdab89;
	C = 0x98badcfe;
	D = 0x10325476;
	total[0] = total[1] = 0;
	buflen = 0;
}

/** Put result from CTX in first 16 bytes following RESBUF.  The result
  must be in little endian byte order.

IMPORTANT: On some systems it is required that RESBUF is correctly
aligned for a 32 bits value.  */
void gott::MD5::read_ctx( Sum& result )
{
	reinterpret_cast<boost::uint32_t*>(result.data)[0] = SWAP (A);
	reinterpret_cast<boost::uint32_t*>(result.data)[1] = SWAP (B);
	reinterpret_cast<boost::uint32_t*>(result.data)[2] = SWAP (C);
	reinterpret_cast<boost::uint32_t*>(result.data)[3] = SWAP (D);
}

/* Process the remaining bytes in the internal buffer and the usual
   prolog according to the standard and write the result to RESBUF.

IMPORTANT: On some systems it is required that RESBUF is correctly
aligned for a 32 bits value.  */
void gott::MD5::finish_ctx ( Sum& result )
{
	/* Take yet unprocessed bytes into account.  */
	boost::uint32_t bytes = buflen;
	size_t pad;

	/* Now count remaining bytes.  */
	total[0] += bytes;
	if (total[0] < bytes)
		++total[1];

	pad = bytes >= 56 ? 64 + 56 - bytes : 56 - bytes;
	std::memcpy (&internal_buffer[bytes], fillbuf, pad);

	/* Put the 64-bit file length in *bits* at the end of the buffer.  */
	*reinterpret_cast<boost::uint32_t*>(&internal_buffer[bytes + pad]) = SWAP (total[0] << 3);
	*reinterpret_cast<boost::uint32_t*>(&internal_buffer[bytes + pad + 4]) = SWAP ((total[1] << 3) |
			(total[0] >> 29));

	/* Process last bytes.  */
	process_block( internal_buffer, bytes + pad + 8 );

	read_ctx( result );
}

/* Compute MD5 message digest for bytes read from STREAM.  The
   resulting message digest number will be written into the 16 bytes
   beginning at RESBLOCK.  */
bool gott::MD5::file( const char *filename, Sum& result )
{
	std::ifstream input( filename );
	if( !input.is_open() )
		return false;

	char buffer[BLOCKSIZE + 72];

	size_t sum;

	bool done = false;
	
	// Initialize the computation context. 
	init();

	while( ! done )
	{
		sum = 0;
		size_t n;
		while( 1 ) 
		{
			size_t n = input.readsome( buffer + sum, BLOCKSIZE - sum );
			sum += n;
			if( sum == BLOCKSIZE ) 
			{
				process_block( buffer, BLOCKSIZE ); 
				break;
			}
			if( n == 0)
			{
				if( !input.good() )
					return false;
				else 
					done = true;

				if( sum )
					process_bytes (buffer, sum );
				break;
			}
		}
	}

	finish_ctx( result );
	return true;
}

/* Compute MD5 message digest for LEN bytes beginning at BUFFER.  The
   result is always in little endian byte order, so that a byte-wise
   output yields to the wanted ASCII representation of the message
   digest.  */
void gott::MD5::buffer( const char *buffer, size_t len, Sum& result )
{
	init();
	/* Process whole buffer but last len % 64 bytes.  */
	process_bytes( buffer, len );

	/* Put result in desired memory area.  */
	return finish_ctx( result );
}


void gott::MD5::process_bytes ( const char *buffer, size_t len )
{
	/* When we already have some bits in our internal buffer concatenate
	   both inputs first.  */
	if( buflen != 0 )
	{
		size_t left_over = buflen;
		size_t add = 128 - left_over > len ? len : 128 - left_over;

		std::memcpy (&internal_buffer[left_over], buffer, add);
		buflen += add;

		if( buflen > 64 )
		{
			process_block( internal_buffer, buflen & ~63 );

			buflen &= 63;
			/* The regions in the following copy operation cannot overlap.  */
			std::memcpy( internal_buffer, &internal_buffer[(left_over + add) & ~63], buflen);
		}

		buffer += add;
		len -= add;
	}

	/* Process available complete blocks.  */
	if (len >= 64)
	{
#if !_STRING_ARCH_unaligned
		/* To check alignment gcc has an appropriate operator.  Other
		   compilers don't.  */
# if __GNUC__ >= 2
#  define UNALIGNED_P(p) (long(p) % __alignof__ (boost::uint32_t) != 0)
# else
#  define UNALIGNED_P(p) (long(p) % sizeof (boost::uint32_t) != 0)
# endif
		if (UNALIGNED_P (buffer))
			while (len > 64)
			{
				std::memcpy( internal_buffer, buffer, 64 );
				process_block( internal_buffer, 64 );
				buffer += 64;
				len -= 64;
			}
		else
#endif
		{
			process_block( buffer, len & ~63 );
			buffer += (len & ~63);
			len &= 63;
		}
	}

	/* Move remaining bytes in internal buffer.  */
	if (len > 0)
	{
		size_t left_over = this->buflen;

		std::memcpy( &internal_buffer[left_over], buffer, len );
		left_over += len;
		if (left_over >= 64)
		{
			process_block( internal_buffer, 64 );
			left_over -= 64;
			std::memcpy( internal_buffer, &internal_buffer[64], left_over );
		}
		buflen = left_over;
	}
}


/* These are the four functions used in the four steps of the MD5 algorithm
   and defined in the RFC 1321.  The first function is a little bit optimized
   (as found in Colin Plumbs public domain implementation).  */
/* #define FF(b, c, d) ((b & c) | (~b & d)) */
#define FF(b, c, d) (d ^ (b & (c ^ d)))
#define FG(b, c, d) FF (d, b, c)
#define FH(b, c, d) (b ^ c ^ d)
#define FI(b, c, d) (c ^ (b | ~d))

/* Process LEN bytes of BUFFER, accumulating context into CTX.
   It is assumed that LEN % 64 == 0.  */

void gott::MD5::process_block (char const* buffer,size_t len)
{
	boost::uint32_t const* words = reinterpret_cast<boost::uint32_t const*>(buffer);
	boost::uint32_t correct_words[16];
	size_t nwords = len / sizeof (boost::uint32_t);
	const boost::uint32_t *endp = words + nwords;

	boost::uint32_t A = this->A;
	boost::uint32_t B = this->B;
	boost::uint32_t C = this->C;
	boost::uint32_t D = this->D;


	/* First increment the byte count.  RFC 1321 specifies the possible
	   length of the file up to 2^64 bits.  Here we only compute the
	   number of bytes.  Do a double word increment.  */
	total[0] += len;
	if (total[0] < len)
		++total[1];

	/* Process all bytes in the buffer with 64 bytes in each round of
	   the loop.  */
	while (words < endp)
	{
		boost::uint32_t *cwp = correct_words;
		boost::uint32_t A_save = A;
		boost::uint32_t B_save = B;
		boost::uint32_t C_save = C;
		boost::uint32_t D_save = D;

		/* First round: using the given function, the context and a constant
		   the next context is computed.  Because the algorithms processing
		   unit is a 32-bit word and it is determined to work on words in
		   little endian byte order we perhaps have to change the byte order
		   before the computation.  To reduce the work for the next steps
		   we store the swapped words in the array CORRECT_WORDS.  */

#define OP(a, b, c, d, s, T)						\
		do								\
		{								\
			a += FF (b, c, d) + (*cwp++ = SWAP (*words)) + T;		\
			++words;							\
			a = rol (a, s);						\
			a += b;							\
		}								\
		while (0)

		/* Before we start, one word to the strange constants.
		   They are defined in RFC 1321 as

		   T[i] = (int) (4294967296.0 * fabs (sin (i))), i=1..64, or
		   perl -e 'foreach(1..64){printf "0x%08x\n", int (4294967296 * abs (sin $_))}'
		   */

		/* Round 1.  */
		OP (A, B, C, D,  7, 0xd76aa478);
		OP (D, A, B, C, 12, 0xe8c7b756);
		OP (C, D, A, B, 17, 0x242070db);
		OP (B, C, D, A, 22, 0xc1bdceee);
		OP (A, B, C, D,  7, 0xf57c0faf);
		OP (D, A, B, C, 12, 0x4787c62a);
		OP (C, D, A, B, 17, 0xa8304613);
		OP (B, C, D, A, 22, 0xfd469501);
		OP (A, B, C, D,  7, 0x698098d8);
		OP (D, A, B, C, 12, 0x8b44f7af);
		OP (C, D, A, B, 17, 0xffff5bb1);
		OP (B, C, D, A, 22, 0x895cd7be);
		OP (A, B, C, D,  7, 0x6b901122);
		OP (D, A, B, C, 12, 0xfd987193);
		OP (C, D, A, B, 17, 0xa679438e);
		OP (B, C, D, A, 22, 0x49b40821);

		/* For the second to fourth round we have the possibly swapped words
		   in CORRECT_WORDS.  Redefine the macro to take an additional first
		   argument specifying the function to use.  */
#undef OP
#define OP(f, a, b, c, d, k, s, T)					\
		do								\
		{								\
			a += f (b, c, d) + correct_words[k] + T;			\
			a = rol (a, s);						\
			a += b;							\
		}								\
		while (0)

		/* Round 2.  */
		OP (FG, A, B, C, D,  1,  5, 0xf61e2562);
		OP (FG, D, A, B, C,  6,  9, 0xc040b340);
		OP (FG, C, D, A, B, 11, 14, 0x265e5a51);
		OP (FG, B, C, D, A,  0, 20, 0xe9b6c7aa);
		OP (FG, A, B, C, D,  5,  5, 0xd62f105d);
		OP (FG, D, A, B, C, 10,  9, 0x02441453);
		OP (FG, C, D, A, B, 15, 14, 0xd8a1e681);
		OP (FG, B, C, D, A,  4, 20, 0xe7d3fbc8);
		OP (FG, A, B, C, D,  9,  5, 0x21e1cde6);
		OP (FG, D, A, B, C, 14,  9, 0xc33707d6);
		OP (FG, C, D, A, B,  3, 14, 0xf4d50d87);
		OP (FG, B, C, D, A,  8, 20, 0x455a14ed);
		OP (FG, A, B, C, D, 13,  5, 0xa9e3e905);
		OP (FG, D, A, B, C,  2,  9, 0xfcefa3f8);
		OP (FG, C, D, A, B,  7, 14, 0x676f02d9);
		OP (FG, B, C, D, A, 12, 20, 0x8d2a4c8a);

		/* Round 3.  */
		OP (FH, A, B, C, D,  5,  4, 0xfffa3942);
		OP (FH, D, A, B, C,  8, 11, 0x8771f681);
		OP (FH, C, D, A, B, 11, 16, 0x6d9d6122);
		OP (FH, B, C, D, A, 14, 23, 0xfde5380c);
		OP (FH, A, B, C, D,  1,  4, 0xa4beea44);
		OP (FH, D, A, B, C,  4, 11, 0x4bdecfa9);
		OP (FH, C, D, A, B,  7, 16, 0xf6bb4b60);
		OP (FH, B, C, D, A, 10, 23, 0xbebfbc70);
		OP (FH, A, B, C, D, 13,  4, 0x289b7ec6);
		OP (FH, D, A, B, C,  0, 11, 0xeaa127fa);
		OP (FH, C, D, A, B,  3, 16, 0xd4ef3085);
		OP (FH, B, C, D, A,  6, 23, 0x04881d05);
		OP (FH, A, B, C, D,  9,  4, 0xd9d4d039);
		OP (FH, D, A, B, C, 12, 11, 0xe6db99e5);
		OP (FH, C, D, A, B, 15, 16, 0x1fa27cf8);
		OP (FH, B, C, D, A,  2, 23, 0xc4ac5665);

		/* Round 4.  */
		OP (FI, A, B, C, D,  0,  6, 0xf4292244);
		OP (FI, D, A, B, C,  7, 10, 0x432aff97);
		OP (FI, C, D, A, B, 14, 15, 0xab9423a7);
		OP (FI, B, C, D, A,  5, 21, 0xfc93a039);
		OP (FI, A, B, C, D, 12,  6, 0x655b59c3);
		OP (FI, D, A, B, C,  3, 10, 0x8f0ccc92);
		OP (FI, C, D, A, B, 10, 15, 0xffeff47d);
		OP (FI, B, C, D, A,  1, 21, 0x85845dd1);
		OP (FI, A, B, C, D,  8,  6, 0x6fa87e4f);
		OP (FI, D, A, B, C, 15, 10, 0xfe2ce6e0);
		OP (FI, C, D, A, B,  6, 15, 0xa3014314);
		OP (FI, B, C, D, A, 13, 21, 0x4e0811a1);
		OP (FI, A, B, C, D,  4,  6, 0xf7537e82);
		OP (FI, D, A, B, C, 11, 10, 0xbd3af235);
		OP (FI, C, D, A, B,  2, 15, 0x2ad7d2bb);
		OP (FI, B, C, D, A,  9, 21, 0xeb86d391);

		/* Add the starting values of the context.  */
		A += A_save;
		B += B_save;
		C += C_save;
		D += D_save;
	}
	this->A = A;
	this->B = B;
	this->C = C;
	this->D = D;

}

void gott::read_from_string( std::string const& source, gott::MD5::Sum & obj )
{
	using namespace boost::spirit;
	uint_parser<gott::boost::uint32_t, 16, 8, 8> hex_4;
	parse_info<const char *> p_info = parse(
			source.c_str(), 
			source.c_str() + source.length(),
			hex_4[ assign_a( reinterpret_cast<gott::boost::uint32_t*>(obj.data)[0] ) ] >> 
			hex_4[ assign_a( reinterpret_cast<gott::boost::uint32_t*>(obj.data)[1] ) ] >> 
			hex_4[ assign_a( reinterpret_cast<gott::boost::uint32_t*>(obj.data)[2] ) ] >> 
			hex_4[ assign_a( reinterpret_cast<gott::boost::uint32_t*>(obj.data)[3] ) ] 
			);
	if( !p_info.hit || p_info.length != 32 )
		throw std::runtime_error( ("Error while parsing md5sum: " + source).c_str() );
}

void gott::write_to_string( std::string & data, gott::MD5::Sum const& obj )
{
	char buffer[33];
	for( int i = 0; i < 16; ++i )
	{
		buffer[i<<1] = "0123456789abcdef"[obj.data[i]>>4];
		buffer[1 + (i<<1)] = "0123456789abcdef"[obj.data[i]%16];
	}
	buffer[32] = 0;
	data = buffer;
}

bool operator==( gott::MD5::Sum const& left, gott::MD5::Sum const & right )
{
	return std::memcmp( left.data, right.data, sizeof(left.data) ) == 0;
}

bool operator!=( gott::MD5::Sum const& left, gott::MD5::Sum const & right )
{
	return std::memcmp( left.data, right.data, sizeof(left.data) ) != 0;
}

std::istream& operator>>( std::istream& input, gott::MD5::Sum & object )
{
	std::string source;
	input >> source;
	try {
		gott::read_from_string( source, object );
		return input;
	}
	catch (std::runtime_error &e )
	{
		for( std::string::const_reverse_iterator b = source.rbegin(), e = source.rend();
				b != e; ++b)
			input.putback( *b );
		input.setstate( std::ios::failbit );
		return input;
	}
}

std::ostream& operator<<( std::ostream& output, gott::MD5::Sum const& object )
{
	for( int i = 0; i < 16; ++i )
	{
		output << "0123456789abcdef"[object.data[i]>>4];
		output << "0123456789abcdef"[object.data[i]%16];
	}

	return output;
}



#ifdef MD5SUM_TEST
#include <iostream>
int main( int argc, char const** argv )
{
	if( argc != 2 )
		std::cout << "Usage: ./md5sum filename" << std::endl;
	
	gott::MD5 test;
	gott::MD5::Sum  result;
	if( test.file( argv[1], result ) )
	std::cout << result << " " << argv[1] << std::endl;
	else 
		std::cout << "EEK :: failire" <<  std::endl;
}
#endif
