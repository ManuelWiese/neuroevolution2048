#include "genome.h"
#include "pool.h"
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

genome::genome(genome &copyGenome) {
    fitness = 0;
    maxneuron = copyGenome.maxneuron;
    globalRank = 0;
    for (auto const& x : copyGenome.mutationRates){
        mutationRates[x.first] = x.second;
    }

    poolPointer = copyGenome.poolPointer;

    for( auto const& x : copyGenome.neurons){
        neurons[x.first] = new neuron();
        neurons[x.first]->transfer = x.second->transfer;
        neurons[x.first]->bias = x.second->bias;
        neurons[x.first]->activated = x.second->activated;
    }

    for( auto const& gene : genes ){
        if( gene->enabled ){
            neurons[gene->out]->addIncoming(gene);
        }
    }

}

genome genome::basicGenome(pool *poolPtr){
    genome outputGenome(poolPtr);
    outputGenome.maxneuron = poolPtr->inputs -1;
    outputGenome.mutate();

    return outputGenome;
}

genome* genome::crossover(genome* genome1, genome* genome2){
    if(genome1->fitness > genome2->fitness){
        genome *tmpGenome = genome1;
        genome1 = genome2;
        genome2 = tmpGenome;
    }
    genome *child = new genome(genome1->poolPointer);

    for( auto const& x : genome1->neurons){
        child->neurons[x.first]->transfer = x.second->transfer;
        child->neurons[x.first]->bias = x.second->bias;
    }

    gene* copyGene;
    std::map<unsigned short, gene*> innovations;

    for( auto const& x : genome2->genes){
        innovations[x->innovation] = x;
    }

    for( auto const& gene1 : genome1->genes){
        auto search = innovations.find(gene1->innovation);
        if( search != innovations.end()){
            gene* gene2 = innovations[gene1->innovation];
            if(gene2->enabled && dis(gen) > 0.5)
                copyGene = new gene(*gene2);
            else
                copyGene = new gene(*gene1);
        } else
            copyGene = new gene(*gene1);
        if( copyGene->enabled )
            child->neurons[copyGene->out]->addIncoming(copyGene);
        child->genes.push_back(copyGene);
    }
    child->maxneuron = genome1->maxneuron;

    for( auto const& x : genome1->mutationRates){
        child->mutationRates[x.first] = x.second;
    }

    return child;
}

double genome::disjoint(genome* genome1, genome* genome2){
    if( genome1->genes.size() == 0 && genome2->genes.size() == 0)
        return 0.0;

    std::vector<unsigned short> innovations;
    for( auto const& x : genome1->genes){
        innovations.push_back(x->innovation);
    }

    unsigned short disjointGenes = genome1->genes.size();

    for( auto const& x : genome2->genes){
        if(std::find(innovations.begin(), innovations.end(), x->innovation) != innovations.end())
            disjointGenes -= 1;
        else
            disjointGenes += 1;
    }
    return disjointGenes / std::max(genome1->genes.size(), genome2->genes.size());
}

double genome::weights(genome* genome1, genome* genome2){
    std::map<unsigned short, gene*> innovations;
    for( auto const& x : genome2->genes)
        innovations[x->innovation] = x;

    double sum = 0.0;
    unsigned short coincident = 0;

    for( auto const& x : genome1->genes){
        auto search =  innovations.find(x->innovation);
        if( search != innovations.end()){
            sum += std::abs(x->weight - search->second->weight);
            coincident += 1;
        }
    }
    if( coincident > 0)
        return sum / coincident;
    return 0.0;
}

bool genome::sameSpecies(genome* genome1, genome* genome2){
    double dd = DELTA_DISJOINT * disjoint(genome1, genome2);
    double dw = DELTA_WEIGHTS * weights(genome1, genome2);

    return( dd + dw < DELTA_THRESHOLD);
}

double genome::calculateNeuron(unsigned short neuronNumber){
    neuron* tmp = neurons[neuronNumber];

    if(tmp->calculated)
        return tmp->value;

    double sum = 0.0;
    for( auto const& x : tmp->incoming)
        sum += x->weight * calculateNeuron(x->into);

    sum += tmp->bias;
    tmp->value = tmp->transfer(sum);
    tmp->calculated = true;
    tmp->activated = tmp->activated || sum != 0.0;

    return tmp->value;
}

double genome::evaluate(std::vector<double> inputs){
    
}
