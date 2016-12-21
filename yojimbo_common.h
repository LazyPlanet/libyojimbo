/*
    Yojimbo Client/Server Network Protocol Library.

    Copyright © 2016, The Network Protocol Company, Inc.

    Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

        1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

        2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer 
           in the documentation and/or other materials provided with the distribution.

        3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived 
           from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
    INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
    USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef YOJIMBO_COMMON_H
#define YOJIMBO_COMMON_H

#include "yojimbo_config.h"
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
    
namespace yojimbo
{
    /**
        Generated a random integer between a and b (inclusive).

        IMPORTANT: This is not a crytographically secure random. It's used only for test functions and in the network simulator.

        For a crytographically secure random, please see yojimbo::RandomBytes which is built on libsodium.

        @param a The minimum integer value to generate.
        @param b The maximum integer value to generate.

        @returns A pseudo random integer value in [a,b].
     */

    inline int random_int( int a, int b )
    {
        assert( a < b );
        int result = a + rand() % ( b - a + 1 );
        assert( result >= a );
        assert( result <= b );
        return result;
    }

    /** 
        Generates a random float between a and b.

        IMPORTANT: This is not a crytographically secure random. It's used only for test functions and in the network simulator.

        For a crytographically secure random, please see yojimbo::RandomBytes which is built on libsodium.

        @param a The minimum integer value to generate.
        @param b The maximum integer value to generate.

        @returns A pseudo random float value in [a,b].
     */

    inline float random_float( float a, float b )
    {
        assert( a < b );
        float random = ( (float) rand() ) / (float) RAND_MAX;
        float diff = b - a;
        float r = random * diff;
        return a + r;
    }

    #ifndef min

    /**
        Template function to get the minimum of two values.

        @param a The first value.
        @param b The second value.

        @returns The minimum of a and b.
     */

    template <typename T> const T & min( const T & a, const T & b )
    {
        return ( a < b ) ? a : b;
    }

    #endif // #ifndef min

    #ifndef max

    /**
        Template function to get the maximum of two values.

        @param a The first value.
        @param b The second value.

        @returns The maximum of a and b.
     */

    template <typename T> const T & max( const T & a, const T & b )
    {
        return ( a > b ) ? a : b;
    }

    #endif // #ifndef max

    /**
        Template function to clamp a value.

        @param value The value to be clamped.
        @param a The minimum value.
        @param b The minimum value.

        @returns The clamped value in [a,b].
     */

    template <typename T> T clamp( const T & value, const T & a, const T & b )
    {
        if ( value < a )
            return a;
        else if ( value > b )
            return b;
        else
            return value;
    }

    /**
        Swaps two values.

        @param a First value.
        @param b Second value.
     */

    template <typename T> void swap( T & a, T & b )
    {
        T tmp = a;
        a = b;
        b = tmp;
    };

    /**
        Get the absolute value.

        @param value The input value.

        @returns The absolute value.
     */

    template <typename T> T abs( const T & value )
    {
        return ( value < 0 ) ? -value : value;
    }

    /**
        Calculate the population count of an unsigned 32 bit integer at compile time.

        Population count is the number of bits in the integer that set to 1.

        See "Hacker's Delight" and http://www.hackersdelight.org/hdcodetxt/popArrayHS.c.txt

        @see yojimbo::Log2
        @see yojimbo::BitsRequired
     */

    template <uint32_t x> struct PopCount
    {
        enum {   a = x - ( ( x >> 1 )       & 0x55555555 ),
                 b =   ( ( ( a >> 2 )       & 0x33333333 ) + ( a & 0x33333333 ) ),
                 c =   ( ( ( b >> 4 ) + b ) & 0x0f0f0f0f ),
                 d =   c + ( c >> 8 ),
                 e =   d + ( d >> 16 ),

            result = e & 0x0000003f 
        };
    };

    /**
        Calculate the log 2 of an unsigned 32 bit integer at compile time.

        @see yojimbo::Log2
        @see yojimbo::BitsRequired
     */

    template <uint32_t x> struct Log2
    {
        enum {   a = x | ( x >> 1 ),
                 b = a | ( a >> 2 ),
                 c = b | ( b >> 4 ),
                 d = c | ( c >> 8 ),
                 e = d | ( d >> 16 ),
                 f = e >> 1,

            result = PopCount<f>::result
        };
    };

    /**
        Calculate the number of bits required to serialize an integer value in [min,max] at compile time.

        @see Log2
        @see PopCount
     */

    template <int64_t min, int64_t max> struct BitsRequired
    {
        static const uint32_t result = ( min == max ) ? 0 : ( Log2<uint32_t(max-min)>::result + 1 );
    };

    /**
        Calculate the population count of an unsigned 32 bit integer.

        The population count is the number of bits in the integer set to 1.

        @param x The input integer value.

        @returns The number of bits set to 1 in the input value.
     */

    inline uint32_t popcount( uint32_t x )
    {
#ifdef __GNUC__
        return __builtin_popcount( x );
#else // #ifdef __GNUC__
        const uint32_t a = x - ( ( x >> 1 )       & 0x55555555 );
        const uint32_t b =   ( ( ( a >> 2 )       & 0x33333333 ) + ( a & 0x33333333 ) );
        const uint32_t c =   ( ( ( b >> 4 ) + b ) & 0x0f0f0f0f );
        const uint32_t d =   c + ( c >> 8 );
        const uint32_t e =   d + ( d >> 16 );
        const uint32_t result = e & 0x0000003f;
        return result;
#endif // #ifdef __GNUC__
    }

    /**
        Calculate the log base 2 of an unsigned 32 bit integer.
    
        @param x The input integer value.

        @returns The log base 2 of the input.
     */

    inline uint32_t log2( uint32_t x )
    {
        const uint32_t a = x | ( x >> 1 );
        const uint32_t b = a | ( a >> 2 );
        const uint32_t c = b | ( b >> 4 );
        const uint32_t d = c | ( c >> 8 );
        const uint32_t e = d | ( d >> 16 );
        const uint32_t f = e >> 1;
        return popcount( f );
    }

    /**
        Calculate the number of bits required to serialize an integer in range [min,max].

        @param min The minimum value.
        @param max The maximum value.

        @returns The number of bits required to serialize the integer.
     */

    inline int bits_required( uint32_t min, uint32_t max )
    {
#ifdef __GNUC__
        return 32 - __builtin_clz( max - min );
#else // #ifdef __GNUC__
        return ( min == max ) ? 0 : log2( max - min ) + 1;
#endif // #ifdef __GNUC__
    }

    /**
        Reverse the order of bytes in a 64 bit integer.
        
        @param value The input value.

        @returns The input value with the byte order reversed.
     */

    inline uint64_t bswap( uint64_t value )
    {
#ifdef __GNUC__
        return __builtin_bswap64( value );
#else // #ifdef __GNUC__
        value = ( value & 0x00000000FFFFFFFF ) << 32 | ( value & 0xFFFFFFFF00000000 ) >> 32;
        value = ( value & 0x0000FFFF0000FFFF ) << 16 | ( value & 0xFFFF0000FFFF0000 ) >> 16;
        value = ( value & 0x00FF00FF00FF00FF ) << 8  | ( value & 0xFF00FF00FF00FF00 ) >> 8;
        return value;
#endif // #ifdef __GNUC__
    }

    /**
        Reverse the order of bytes in a 32 bit integer.
        
        @param value The input value.

        @returns The input value with the byte order reversed.
     */

    inline uint32_t bswap( uint32_t value )
    {
#ifdef __GNUC__
        return __builtin_bswap32( value );
#else // #ifdef __GNUC__
        return ( value & 0x000000ff ) << 24 | ( value & 0x0000ff00 ) << 8 | ( value & 0x00ff0000 ) >> 8 | ( value & 0xff000000 ) >> 24;
#endif // #ifdef __GNUC__
    }

    /**
        Reverse the order of bytes in a 16 bit integer.
        
        @param value The input value.

        @returns The input value with the byte order reversed.
     */

    inline uint16_t bswap( uint16_t value )
    {
        return ( value & 0x00ff ) << 8 | ( value & 0xff00 ) >> 8;
    }

    /**
        Template to convert an integer value from local byte order to network byte order.

        IMPORTANT: Yojimbo defines network byte order to be little endian, because most machines running yojimbo are little endian, this creates the least amount of work.

        @param value The input value in local byte order. Supported integer types: uint64_t, uint32_t, uint16_t.

        @returns The input value converted to network byte order. If this processor is little endian the output is the same as the input. If the processor is big endian, the output is the input byte swapped.

        @see yojimbo::bswap
     */

    template <typename T> T host_to_network( T value )
    {
#if YOJIMBO_BIG_ENDIAN
        return bswap( value );
#else // #if YOJIMBO_BIG_ENDIAN
        return value;
#endif // #if YOJIMBO_BIG_ENDIAN
    }

    /**
        Template to convert an integer value from network byte order to local byte order.

        IMPORTANT: Yojimbo defines network byte order to be little endian, because most machines running yojimbo are little endian, this creates the least amount of work.

        @param value The input value in network byte order. Supported integer types: uint64_t, uint32_t, uint16_t.

        @returns The input value converted to local byte order. If this processor is little endian the output is the same as the input. If the processor is big endian, the output is the input byte swapped.

        @see yojimbo::bswap
     */

    template <typename T> T network_to_host( T value )
    {
#if YOJIMBO_BIG_ENDIAN
        return bswap( value );
#else // #if YOJIMBO_BIG_ENDIAN
        return value;
#endif // #if YOJIMBO_BIG_ENDIAN
    }

    /** 
        Compares two 16 bit sequence numbers and returns true if the first one is greater than the second (considering wrapping).

        IMPORTANT: This is not the same as s1 > s2!

        Greater than is defined specially to handle wrapping sequence numbers. 

        If the two sequence numbers are close together, it is as normal, but they are far apart, it is assumed that they have wrapped around.

        Thus, sequence_greater_than( 1, 0 ) returns true, and so does sequence_greater_than( 0, 65535 )!

        @param s1 The first sequence number.
        @param s2 The second sequence number.

        @returns True if the s1 is greater than s2, with sequence number wrapping considered.
     */

    inline bool sequence_greater_than( uint16_t s1, uint16_t s2 )
    {
        return ( ( s1 > s2 ) && ( s1 - s2 <= 32768 ) ) || 
               ( ( s1 < s2 ) && ( s2 - s1  > 32768 ) );
    }

    /** 
        Compares two 16 bit sequence numbers and returns true if the first one is less than the second (considering wrapping).

        IMPORTANT: This is not the same as s1 < s2!

        Greater than is defined specially to handle wrapping sequence numbers. 

        If the two sequence numbers are close together, it is as normal, but they are far apart, it is assumed that they have wrapped around.

        Thus, sequence_less_than( 0, 1 ) returns true, and so does sequence_greater_than( 65535, 0 )!

        @param s1 The first sequence number.
        @param s2 The second sequence number.

        @returns True if the s1 is less than s2, with sequence number wrapping considered.
     */

    inline bool sequence_less_than( uint16_t s1, uint16_t s2 )
    {
        return sequence_greater_than( s2, s1 );
    }

    /**
        Convert a signed integer to an unsigned integer with zig-zag encoding.

        0,-1,+1,-2,+2... becomes 0,1,2,3,4 ...

        @param n The input value

        @returns The input value converted from signed to unsigned with zig-zag encoding.
     */

    inline int signed_to_unsigned( int n )
    {
        return ( n << 1 ) ^ ( n >> 31 );
    }

    /**
        Convert an unsigned integer to as signed integer with zig-zag encoding.

        0,1,2,3,4... becomes 0,-1,+1,-2,+2...

        @param n The input value

        @returns The input value converted from unsigned to signed with zig-zag encoding.
     */

    inline int unsigned_to_signed( uint32_t n )
    {
        return ( n >> 1 ) ^ ( -int32_t( n & 1 ) );
    }

    void compress_packet_sequence( uint64_t sequence, uint8_t & prefix_byte, int & num_sequence_bytes, uint8_t * sequence_bytes );

    int get_packet_sequence_bytes( uint8_t prefix_byte );

    uint64_t decompress_packet_sequence( uint8_t prefix_byte, const uint8_t * sequence_bytes );

    uint32_t calculate_crc32( const uint8_t *buffer, size_t length, uint32_t crc32 = 0 );

    uint32_t hash_data( const uint8_t * data, uint32_t length, uint32_t hash );

    uint32_t hash_string( const char string[], uint32_t hash );

    uint64_t murmur_hash_64( const void * key, uint32_t length, uint64_t seed );

    void print_bytes( const char * label, const uint8_t * data, int data_bytes );

    int base64_encode_string( const char * input, char * output, int output_size );

    int base64_decode_string( const char * input, char * output, int output_size );

    int base64_encode_data( const uint8_t * input, int input_length, char * output, int output_size );

    int base64_decode_data( const char * input, uint8_t * output, int output_size );

    void debug_printf( const char * format, ... );
}

#endif // #ifndef YOJIMBO_COMMON_H
