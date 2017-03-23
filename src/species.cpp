#include "species.h"

species::species(){
    topFitness = std::numeric_limits<double>::min();
    staleness = 0;
    averageFitness = 0;
}

void species::calculateAverageFitness(){
    double tmpSum = 0;
    for ( auto &genome : genomes ) {
        tmpSum += genome->globalRank;
    }
    averageFitness = tmpSum / genomes.size();
}


genome species::breedChild(){
    genome child;
    if( dis(gen) < CROSSOVER_CHANCE) {
        child = genome::crossover( genomes[(int)genomes.size() * dis(gen)],
                                   genomes[(int)genomes.size() * dis(gen)]
                               );
    } else
        child = genome(*genomes[(int)genomes.size() * dis(gen)]);
    child.mutate();
    return child;
}
