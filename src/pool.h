#ifndef POOL_H
#define POOL_H

#include "species.h"
#include <string>
#include <vector>

class pool{
public:
    std::string             timestamp;
    std::vector<species*>   speciesVector;
    unsigned short          generation;
    unsigned int            innovation;
    unsigned short          currentSpecies;
    unsigned short          currentGenome;
    double                  maxFitness;
    unsigned short          population;
    unsigned short          inputs;
    unsigned short          outputs;
    bool                    firstOfGeneration;

                        pool(unsigned short inputs, unsigned short outputs, unsigned short population);
                        ~pool();
    unsigned int        newInnovation();
    void                rankGenomes();
    double              getAverageFitness();
    void                cullSpecies(bool cutToMin);
    void                removeStaleSpecies();
    void                removeWeakSpecies();
    void                addToSpecies(genome* child);
    void                newGeneration();
    bool                checkDistance();
    void                nextGenome();
};

#endif //POOL_H
