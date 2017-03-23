#ifndef SPECIES_H
#define SPECIES_H

#include <vector>
#include "genome.h"

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
