/*
===========================================================================
Ising cluster algorithms
Copyright (C) 2014-2015  Manuel Wiese

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#include "random_t.h"

/*
================
random_t

Constructor, initialize the random number generator
with default-seed and discard the first DISCARD_STARTUP numbers
================
*/
random_t::random_t()
{
#ifdef DEBUG
    calls = 0;
#endif
    randomNumber[0] = DEFAULT_SEED;
    randomNumber[1] = 1812433253UL * (randomNumber[0] ^ (randomNumber[0] >> 30)) + 1;

    seed = DEFAULT_SEED;
    discard(DISCARD_STARTUP);
}

/*
================
random_t(seed)

Constructor, initialize the random number generator
with seed and discard the first DISCARD_STARTUP numbers
================
*/
random_t::random_t(uint64_t seed)
{
#ifdef DEBUG
    calls = 0;
#endif

    if( seed == TIMED_RANDOM ) {
        randomNumber[0] = time(0);
        seed = randomNumber[0];
    } else {
        randomNumber[0] = seed;
        this->seed = seed;
    }
    randomNumber[1] = 1812433253UL * (randomNumber[0] ^ (randomNumber[0] >> 30)) + 1;
    discard(DISCARD_STARTUP);
}

/*
================
random_t(state)

Constructor, initialize the random number generator
with two random numbers(full state). No discard, this
method can be used to set the rng to a previous state.
================
*/
random_t::random_t(uint64_t seed, uint64_t random0, uint64_t random1)
{
#ifdef DEBUG
    calls = 0;
    std::cout << "Set rng state: " << seed << "	" << random0 << "	" << random1 << std::endl;
#endif
    this->seed = seed;
    randomNumber[0] = random0;
    randomNumber[1] = random1;
}

/*
================
reset

Resets the prng to the initial state(post constructor)
================
*/
void random_t::reset()
{
    randomNumber[0] = seed;
    randomNumber[1] = 1812433253UL * (randomNumber[0] ^ (randomNumber[0] >> 30)) + 1;
    discard(DISCARD_STARTUP);
}

/*
================
setSeed

Sets a new seed and initializes the prng.
reset() will reset the prng to this state.
================
*/
void random_t::setSeed(uint64_t seed)
{
    randomNumber[0] = seed;
    randomNumber[1] = 1812433253UL * (randomNumber[0] ^ (randomNumber[0] >> 30)) + 1;
    this->seed = seed;
    discard(DISCARD_STARTUP);
}

/*
================
randn

Pseudorandomnumber-generator using
the xorshift128+ algorithm.
================
*/
uint64_t random_t::randn() {
#ifdef DEBUG
    calls++;
#endif
    uint64_t random1 = randomNumber[ 0 ];
    const uint64_t random0 = randomNumber[ 1 ];
    randomNumber[ 0 ] = random0;
    random1 ^= random1 << 23; // a
    return ( randomNumber[ 1 ] = ( random1 ^ random0 ^ ( random1 >> 17 ) ^ ( random0 >> 26 ) ) ) + random0; // b, c
}

uint8_t random_t::randn4(uint8_t maxint){
    if(!rand4Vector.size()){
        for(unsigned char index = 0; index < 128; ++index){
            uint64_t random = randn();
            for(unsigned char j = 0; j < 16; ++j){
                rand4Vector.push_back(random & 0x000FULL);
                random = random >> 4;
            }
        }
    }
    uint8_t output = rand4Vector.back();
    rand4Vector.pop_back();
    if(output > maxint)
        return randn4(maxint);
    return output;
}

/*
================
randn(uint_32)

Returns a random number between
0 and maxint ( {0, 1, ..., maxint-1} )
================
*/
uint32_t random_t::randn(uint32_t maxint) {
    return floor( rand() * (maxint) );
}

/*
================
rand

Returns a floating point number between 0 and 1( [0,1) )
================
*/
double random_t::rand() {
    return ((double)randn()-1)/UINT64_MAX;
}

/*
================
discard

Calculates the next n random numbers
and discards them.
================
*/
void random_t::discard(uint32_t n)
{
    for( unsigned int i = 0; i < n; i++ )
        randn();
}

/*
================
operator<<

Streams the current state of the rng.
This can be used to save the current state.
================
*/
std::ostream& operator<<(std::ostream& os, const random_t& s) {
    os << s.seed << "\n";
    os << s.randomNumber[0] << "\n";
    os << s.randomNumber[1] << "\n";
    return os;
}
