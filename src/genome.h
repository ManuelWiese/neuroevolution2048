#ifndef GENOME_H
#define  GENOME_H

#include <vector>
#include <string>
#include <map>
#include <random>
#include <algorithm>
#include <cmath>
#include "neuron.h"
#include "config.h"

extern std::mt19937 gen;
extern std::uniform_real_distribution<> dis;

//forward declaration of pool
class pool;

class genome{
public:
    std::vector<gene*>                  genes;
    double                              fitness;
    unsigned short                      maxneuron;
    unsigned short                      globalRank;
    std::map<std::string, double>       mutationRates;
    pool                                *poolPointer;
    std::map<unsigned short, neuron*>   neurons;

                        genome(pool *poolPtr);
                        genome(genome &copyGenome);
    static genome       basicGenome(pool *poolPointer);

    static genome*      crossover(genome *genome1, genome *genome2);
    static double       disjoint(genome *genome1, genome *genome2);
    static double       weights(genome *genome1, genome *genome2);
    static bool         sameSpecies(genome *genome1, genome *genome2);

    double              calculateNeuron(unsigned short neuronNumber);
    double              evaluate(std::vector<double> inputs);
    bool                containsGene(gene *inputGene);
    unsigned short      randomNeuron(bool includeInput);
    bool                isInputNeuron(unsigned short neuronNumber);
    bool                isOutputNeuron(unsigned short neuronNumber);
    bool                areConnected(unsigned short neuron1, unsigned short neuron2);
    bool                linkAllowed(unsigned short neuron1, unsigned short neuron2);

    void                mutate();
    void                weightMutate();
    void                linkMutate();
    void                biasMutate();
    void                nodeMutate();
    void                enableToDisableMutate();
    void                disableToEnableMutate();
    void                transferMutate();

};

#endif //GENOME_H
