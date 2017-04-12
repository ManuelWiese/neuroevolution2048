#ifndef SPECIES_H
#define SPECIES_H

#include <vector>
#include "genome.h"
#include <limits>
#include <random>
#include <cmath>

extern std::mt19937 generator;
extern std::uniform_real_distribution<> distribution;

class species{
public:
    double                  topFitness;
    unsigned short          staleness;
    std::vector<genome*>    genomes;
    double                  averageFitness;

                    species();
    void            calculateAverageFitness();
    genome*         breedChild();

};

#endif //SPECIES_H
