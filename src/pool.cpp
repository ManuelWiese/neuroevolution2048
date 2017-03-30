#include "genome.h"
#include "pool.h"
#include <ctime>
#include <limits>
#include <algorithm>
#include <iostream>

pool::pool(unsigned short inputs, unsigned short outputs, unsigned short population = POPULATION){
    char buffer[128];
    int len;
    len = std::sprintf(buffer, "%f_%f_%f_%f_%f_%f_%f_%f", LINK_MUTATION_CHANCE, NODE_MUTATION_CHANCE, BIAS_MUTATION_CHANCE, 
                                                    WEIGHT_MUTATION_CHANCE, TRANSFER_MUTATION_CHANCE, DISABLE_MUTATION_CHANCE,
                                                    ENABLE_MUTATION_CHANCE, DELETE_MUTATION_CHANCE);
    printf("%s", buffer);
    //TODO: create timestamp using strstreamer?
    timestamp.assign(buffer, len);
    generation = 0;
    innovation = outputs;
    currentSpecies = 0;
    currentGenome = 0;
    maxFitness = std::numeric_limits<double>::min();
    this->population = population;
    this->inputs = inputs;
    this->outputs = outputs;

    for(unsigned short i = 0; i < population; i++){
        addToSpecies(genome::basicGenome(this));
    }
}

pool::~pool(){
    for(auto const& spec : speciesVector){
        for(auto const& genom : spec->genomes){
            for(auto const& gen: genom->genes)
                delete gen;
            for(auto const& neur : genom->neurons)
                delete neur.second;
            delete genom;
        }
        delete spec;
    }
}

unsigned int pool::newInnovation(){
    innovation += 1;
    return innovation;
}

static bool compareByFitness(genome* a, genome* b){
    return a->fitness < b->fitness;
}

void pool::rankGenomes(){
    std::vector<genome*> tmpGenomes;
    for(auto const& spec : speciesVector){
        for(auto const& genom : spec->genomes){
            tmpGenomes.push_back(genom);
        }
    }
    std::sort(tmpGenomes.begin(), tmpGenomes.end(), compareByFitness);

    for( unsigned short i = 0; i < tmpGenomes.size(); i++){
        tmpGenomes[i]->globalRank = i + 1;
    }
}

double pool::getAverageFitness(){
    double sum = 0;
    for(auto const& spec : speciesVector)
        sum += spec->averageFitness;
    return sum/population;
}

static bool compareByFitnessDescending(genome* a, genome* b){
    return b->fitness < a->fitness;
}

void pool::cullSpecies(bool cutToOne){
    for(auto const& spec : speciesVector){
        std::sort(spec->genomes.begin(), spec->genomes.end(), compareByFitnessDescending);
        unsigned short remaining;
        if(cutToOne)
            remaining = 1;
        else
            remaining = ceil(spec->genomes.size() / 2.0);

        for(unsigned int i = remaining; i < spec->genomes.size(); i++){
            for(auto const& gen : spec->genomes[i]->genes){
                delete gen;
            }
            for(auto const& neur : spec->genomes[i]->neurons){
                delete neur.second;
            }
            delete spec->genomes[i];
        }
        spec->genomes.erase(spec->genomes.begin()+remaining, spec->genomes.end());
    }
}

/*static bool compareSpeciesByTopFitness(species* a, species* b){
    return b->topFitness < a->topFitness;
}*/

void pool::removeStaleSpecies(){
    std::vector<species*> survived;
    for(auto const& spec : speciesVector){
        std::sort(spec->genomes.begin(), spec->genomes.end(), compareByFitnessDescending);
        if(spec->genomes[0]->fitness > spec->topFitness){
            spec->topFitness = spec->genomes[0]->fitness;
            spec->staleness = 0;
        } else {
            spec->staleness += 1;
        }
        if(spec->staleness < STALE_SPECIES || spec->topFitness >= maxFitness)
            survived.push_back(spec);
        else {
            for(auto const& genom : spec->genomes){
                for(auto const& gen : genom->genes){
                    delete gen;
                }
                for(auto const& neur : genom->neurons){
                    delete neur.second;
                }
                delete genom;
            }
            delete spec;
        }
    }
    if(!survived.size()){
        //Since everything is free, this cannot work anymore
        //std::sort(speciesVector.begin(), speciesVector.end(), compareSpeciesByTopFitness);
        //survived.push_back(speciesVector[0]);
        printf("ERROR: survived.size() == 0");
        exit;
    }
    speciesVector = survived;
}

void pool::removeWeakSpecies(){
    std::vector<species*> survived;
    double sum = getAverageFitness();
    for(auto const& spec : speciesVector){
        if(floor(spec->averageFitness/sum) >= 1)
            survived.push_back(spec);
        else{
            for(auto const& genom : spec->genomes){
                for(auto const& gen : genom->genes){
                    delete gen;
                }
                for(auto const& neur : genom->neurons){
                    delete neur.second;
                }
                delete genom;
            }
            delete spec;
        }
    }
    speciesVector = survived;
}

void pool::addToSpecies(genome* child){
    for(auto const& spec : speciesVector){
        if(genome::sameSpecies(spec->genomes[0], child)){
            spec->genomes.push_back(child);
            return;
        }
    }
    species* newSpecies = new species();
    newSpecies->genomes.push_back(child);
    speciesVector.push_back(newSpecies);
}

void pool::newGeneration(){
    cullSpecies(false);
    rankGenomes();
    removeStaleSpecies();
    rankGenomes();

    for(auto const& spec : speciesVector)
        spec->calculateAverageFitness();

    removeWeakSpecies();
    double avgFitness = getAverageFitness();

    std::vector<genome*> children;
    for(auto const& spec : speciesVector){
        short breed = floor(spec->averageFitness/avgFitness) - 1;
        for( unsigned short i = 0; i < breed; i++){
            children.push_back(spec->breedChild());
        }
    }

    cullSpecies(true);

    while(children.size() + speciesVector.size() < population)
        children.push_back(speciesVector[(int)(dis(gen)*speciesVector.size())]->breedChild());

    for(auto const& child : children)
        addToSpecies(child);

    generation += 1;
}

void pool::nextGenome(){
    std::vector<genome*> currentGenomes = speciesVector[currentSpecies]->genomes;
    printf("%d  %d  %d  %f  %f\n", generation, currentSpecies, currentGenome, currentGenomes[currentGenome]->fitness, maxFitness);
    currentGenome += 1;
    if(currentGenome >= currentGenomes.size()){
        currentGenome = 0;
        currentSpecies += 1;
        if(currentSpecies >= speciesVector.size()){
            currentSpecies = 0;
            //TODO: insert save here
            newGeneration();
        }
    }
}
