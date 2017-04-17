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
    if( rng[0].rand() < CROSSOVER_CHANCE) {
        child = genome::crossover(genomes[(int)(genomes.size() * rng[0].rand())],
                                  genomes[(int)(genomes.size() * rng[0].rand())]
                                 );
    } else
        child = new genome(*genomes[(int)(genomes.size() * rng[0].rand())]);
    child->mutate();
    return child;
}

std::ostream& operator<<(std::ostream& os, const species& s){
    os << s.topFitness << std::endl;
    os << s.staleness << std::endl;
    os << s.averageFitness << std::endl;

    os << s.genomes.size() << std::endl;
    for(auto const& genom : s.genomes)
        os << *genom;

    return os;
}

std::istream& operator>>(std::istream& is, species& s){
    is >> s.topFitness;
    is >> s.staleness;
    is >> s.averageFitness;

    unsigned int genomesSize;
    is >> genomesSize;
    for(unsigned int i = 0; i < genomesSize; ++i){
        genome* genomePointer = new genome();
        is >> *genomePointer;
        s.genomes.push_back(genomePointer);
    }
    return is;
}
