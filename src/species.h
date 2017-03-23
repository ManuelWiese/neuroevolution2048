#ifndef SPECIES_H
#define SPECIES_H

#include <vector>
#include "genome.h"
#include <limits>
#include <random>
#include <cmath>

extern std::mt19937 gen;
extern std::uniform_real_distribution<> dis;

class species{
public:
    double                  topFitness;
    unsigned short          staleness;
    std::vector<genome*>    genomes;
    double                  averageFitness;

                    species();
    void            calculateAverageFitness();
    genome          breedChild();

};

#endif //SPECIES_H
