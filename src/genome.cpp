#include "genome.h"
#include <limits>

genome::genome(pool *poolPtr){
    fitness = std::numeric_limits<double>::min();
    maxneuron = 0;
    globalRank = 0;

    mutationRates["weight"] = WEIGHT_MUTATION_CHANCE;
    mutationRates["link"] = LINK_MUTATION_CHANCE;
    mutationRates["bias"] = BIAS_MUTATION_CHANCE;
    mutationRates["node"] = NODE_MUTATION_CHANCE;
    mutationRates["enable"] = ENABLE_MUTATION_CHANCE;
    mutationRates["disable"] = DISBALE_MUTATION_CHANCE;
    mutationRates["transfer"] = TRANSFER_MUTATION_CHANCE;
    mutationRates["step"] = STEPSIZE;

    poolPointer = poolPtr;

    for( unsigned int i = 0; i < poolPointer->inputs; i++){
        neurons[i] = new neuron();
    }

    for( unsigned int i = 0; i < poolPointer->outputs; i++){
        neurons[i + MAX_NODES - poolPointer->outputs] = new neuron();
    }
}
