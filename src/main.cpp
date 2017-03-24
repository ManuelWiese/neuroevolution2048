#include <random>

#include "neuron.h"
#include "gene.h"
#include "genome.h"
#include "pool.h"
#include "species.h"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0, 1);

int main() {
    pool mainPool(1, 1, 300);
    while(true){
        genome* currentGenome = mainPool.speciesVector[mainPool.currentSpecies]->genomes[mainPool.currentGenome];
        std::vector<double> input;
        std::vector<double> result;
        input.push_back(dis(gen));
        result.push_back(sin(input[0]));
        std::vector<double> output = currentGenome->evaluate(input);
        double fitness = 1.0/(pow(result[0]-output[0],2));
        currentGenome->fitness = fitness;
        if(fitness > mainPool.maxFitness)
            mainPool.maxFitness = fitness;
        mainPool.nextGenome();
    }
}
