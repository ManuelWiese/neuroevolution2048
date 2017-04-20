#ifndef SPECIES_H
#define SPECIES_H

#include <vector>
#include "genome.h"
#include "random_t.h"
#include <limits>
#include <random>
#include <cmath>

extern std::mt19937 generator;
extern std::uniform_real_distribution<> distribution;

extern std::vector<random_t> rng;

class species{
public:
    double                  topFitness;
    unsigned short          staleness;
    double                  averageFitness;
    std::vector<genome*>    genomes;

    static unsigned int 	number;
    unsigned int 			speciesNumber;
    
                    species();
    void            calculateAverageFitness();
    genome*         breedChild();

    friend std::ostream& operator<<(std::ostream& os, const species& s);
    friend std::istream& operator>>(std::istream& is, species& s);
};

#endif //SPECIES_H
