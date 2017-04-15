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

#ifndef RANDOM_T_H
#define RANDOM_T_H

#include <cstdint>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <iostream>
#include <vector>

#define DISCARD_STARTUP 10000
#define DEFAULT_SEED    12345
#define TIMED_RANDOM    0

class random_t
{
public:
    random_t();
    random_t(uint64_t seed);
    random_t(uint64_t seed, uint64_t random0, uint64_t random1);
    void        reset();
    void        setSeed(uint64_t seed);
    uint64_t    randn();
    uint32_t    randn(uint32_t maxint);
    uint8_t     randn4(uint8_t maxint);
    double      rand();
    void        discard(uint32_t);

#ifdef DEBUG
    uint32_t    calls;
#endif
private:
    uint64_t    randomNumber[2];
    uint64_t    seed;

    std::vector<uint8_t> rand4Vector;

    friend std::ostream& operator<<(std::ostream& os, const random_t& s);
};

#endif // RANDOM_T_H
