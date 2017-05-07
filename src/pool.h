#ifndef POOL_H
#define POOL_H

#include "species.h"
#include <string>
#include <vector>
#include <iostream>

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
    double                  targetPrecision;
    double                  deltaThreshold;
    
                        pool();
                        pool(unsigned short inputs, unsigned short outputs, unsigned short population);
                        ~pool();
    unsigned int        newInnovation();
    void                rankGenomes();
    double              getAverageFitness();
    void                cullSpecies(bool cutToMin);
    void                removeStaleSpecies();
    void                removeWeakSpecies();
    void                addToSpecies(genome* child);
    void                setMaxFitness();
    void                writeStats();
    void                newGeneration();
    bool                setPrecision();
    void                checkVariance();
    void                nextGenome();
    void                save();
    static pool         load(std::string filename);

    friend std::ostream& operator<<(std::ostream& os, const pool& p);
    friend std::istream& operator>>(std::istream& is, pool& p);

};

#endif //POOL_H
