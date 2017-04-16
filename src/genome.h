#ifndef GENOME_H
#define  GENOME_H

#include <vector>
#include <string>
#include <map>
#include <random>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include "neuron.h"
#include "gene.h"
#include "random_t.h"
#include "config.h"

extern std::mt19937 generator;
extern std::uniform_real_distribution<> distribution;

extern std::vector<random_t> rng;

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
    std::vector<neuron*>                neurons;

    std::vector<double>                 scores;
    std::vector<unsigned char>          maxTile;
    double                              precision;
    double                              targetPrecision;
    bool                                calculateScore;

    unsigned char                       maxTileSeen;
    std::vector<uint8_t>                inputActivated;
    std::vector<double*>                inputValue;
    std::vector<double*>                resetValueVector;


                        genome(pool *poolPtr, bool createNeurons);
                        genome(genome &copyGenome);
    static genome*      basicGenome(pool *poolPointer);

    static genome*      crossover(genome *genome1, genome *genome2);
    static double       disjoint(genome *genome1, genome *genome2);
    static double       weights(genome *genome1, genome *genome2);
    static double       bias(genome *genome1, genome *genome2);
    static double       transfer(genome *genome1, genome *genome2);
    static double       rates(genome *genome1, genome *genome2);
    static bool         sameSpecies(genome *genome1, genome *genome2);

    double              calculateNeuron(unsigned short neuronNumber);
    void                evaluate(uint64_t board, std::vector<double> &output);
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
    void                deleteDisabledMutate();

    void                calculateFitness();
};

#endif //GENOME_H
