#include "genome.h"
#include "pool.h"
#include <ctime>
#include <limits>
#include <algorithm>
#include <iostream>

pool::pool(unsigned short inputs, unsigned short outputs, unsigned short population = POPULATION){
    char buffer[256];
    int len;
    len = std::sprintf(buffer, "%sPOP%d_RUNS%d_DD%.3f_DW%.3fDB%.3f_DT%.3f_DR%.3f_THR%.3f_WR%.3f_BR%.3f_CULLMIN%d_STALE%d_PC%.3f_CC%.3f_LI%.3f_NO%.3f_BI%.3f_WE%.3f_TR%.3f_DI%.3f_EN%.3f_DE%.3f_STEP%.3f",
                                NAME_PREFIX,
                                POPULATION,
                                RUNS_PER_NETWORK,
                                DELTA_DISJOINT,
                                DELTA_WEIGHTS,
                                DELTA_BIAS,
                                DELTA_TRANSFER,
                                DELTA_RATES,
                                DELTA_THRESHOLD,
                                WEIGHT_RANGE,
                                BIAS_RANGE,
                                CULL_MINIMUM,
                                STALE_SPECIES,
                                PERTUBCHANCE,
                                CROSSOVER_CHANCE,
                                LINK_MUTATION_CHANCE,
                                NODE_MUTATION_CHANCE,
                                BIAS_MUTATION_CHANCE,
                                WEIGHT_MUTATION_CHANCE,
                                TRANSFER_MUTATION_CHANCE,
                                DISABLE_MUTATION_CHANCE,
                                ENABLE_MUTATION_CHANCE,
                                DELETE_MUTATION_CHANCE,
                                STEPSIZE
                                );
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
    firstOfGeneration = false;

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
                delete neur;
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

void pool::cullSpecies(bool cutToMin){
    for(auto const& spec : speciesVector){
        std::sort(spec->genomes.begin(), spec->genomes.end(), compareByFitnessDescending);
        unsigned short remaining;
        if(cutToMin)
            remaining = CULL_MINIMUM;
        else
            remaining = ceil(spec->genomes.size() / 2.0);

        for(unsigned int i = remaining; i < spec->genomes.size(); i++){
            for(auto const& gen : spec->genomes[i]->genes){
                delete gen;
            }
            for(auto const& neur : spec->genomes[i]->neurons){
                delete neur;
            }
            delete spec->genomes[i];
        }
        if(remaining < spec->genomes.size())
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
                    delete neur;
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
                    delete neur;
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

void pool::setMaxFitness(){
    for(auto const& spec : speciesVector){
        for(auto const& genom : spec->genomes){
            if(genom->fitness > maxFitness)
                maxFitness = genom->fitness;
        }
    }
}

void pool::newGeneration(){
    setMaxFitness();
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
        short breed = floor(spec->averageFitness/avgFitness) - std::min((int)spec->genomes.size(), CULL_MINIMUM);
        printf("breed: %d\n", breed);
        for( unsigned short i = 0; i < breed; i++){
            children.push_back(spec->breedChild());
        }
    }

    cullSpecies(true);
    unsigned short remaining = 0;
    for(auto const& spec : speciesVector){
        remaining += spec->genomes.size();
    }
    printf("children.size() = %ld\n", children.size());
    printf("remaining after cull: %d\n", remaining);
    printf("speciesVector.size() = %ld\n", speciesVector.size());
    while(children.size() + remaining < population)
        children.push_back(speciesVector[(int)(dis(gen)*speciesVector.size())]->breedChild());

    for(auto const& child : children)
        addToSpecies(child);

    generation += 1;
}

bool pool::checkDistance(){
    std::vector<genome*> tmpGenomes;
    for(auto const& spec : speciesVector){
        for(auto const& genom : spec->genomes){
            tmpGenomes.push_back(genom);
        }
    }
    std::sort(tmpGenomes.begin(), tmpGenomes.end(), compareByFitness);
    bool reRun = false;
    for(unsigned short i = 0; i < tmpGenomes.size(); ++i){
        if(i > 0){
            double distance = tmpGenomes[i]->fitness - tmpGenomes[i-1]->fitness;
            if(distance < tmpGenomes[i]->fitness * tmpGenomes[i]->precision){
                tmpGenomes[i]->calculateScore = true;
            }
        }
        if(i < tmpGenomes.size() - 1){
            double distance = tmpGenomes[i+1]->fitness - tmpGenomes[i]->fitness;
            if(distance < tmpGenomes[i]->fitness * tmpGenomes[i]->precision){
                tmpGenomes[i]->calculateScore = true;
            }
        }
        if(tmpGenomes[i]->calculateScore){
            tmpGenomes[i]->precision /= 2.0;
            reRun = true;
        }
    }
    return reRun;
}

void pool::nextGenome(){
    std::vector<genome*> currentGenomes = speciesVector[currentSpecies]->genomes;
    printf("%d  %d  %d  %f  %f  %d\n", generation, currentSpecies, currentGenome, currentGenomes[currentGenome]->fitness, maxFitness, currentGenomes[currentGenome]->runCounter);
    currentGenome += 1;
    firstOfGeneration = false;
    if(currentGenome >= currentGenomes.size()){
        currentGenome = 0;
        currentSpecies += 1;
        if(currentSpecies >= speciesVector.size()){
            currentSpecies = 0;
            //TODO: insert save here
            if(!checkDistance()){
                newGeneration();
                firstOfGeneration = true;
                for(auto const& spec : speciesVector){
                    for(auto const& genom : spec->genomes){
                        genom->calculateScore = true;
                    }
                }
            }
        }
    }
}
