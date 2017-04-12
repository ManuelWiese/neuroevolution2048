#include "species.h"

species::species(){
    topFitness = std::numeric_limits<double>::min();
    staleness = 0;
    averageFitness = 0;
}

void species::calculateAverageFitness(){
    double tmpSum = 0;
    for ( auto &genome : genomes ){
        tmpSum += genome->globalRank;
    }
    averageFitness = tmpSum / genomes.size();
}


genome* species::breedChild(){
    genome *child;
    if( distribution(generator) < CROSSOVER_CHANCE) {
        child = genome::crossover(genomes[(int)(genomes.size() * distribution(generator))],
                                  genomes[(int)(genomes.size() * distribution(generator))]
                                 );
    } else
        child = new genome(*genomes[(int)(genomes.size() * distribution(generator))]);
    child->mutate();
    return child;
}
