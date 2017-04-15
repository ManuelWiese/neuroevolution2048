#include "genome.h"
#include "pool.h"
#include <limits>
#include <utility>

genome::genome(pool *poolPtr, bool createNeurons = true){
    fitness = std::numeric_limits<double>::min();
    maxneuron = poolPtr->inputs + poolPtr->outputs - 1;
    globalRank = 0;

    mutationRates["weight"] = WEIGHT_MUTATION_CHANCE;
    mutationRates["link"] = LINK_MUTATION_CHANCE;
    mutationRates["bias"] = BIAS_MUTATION_CHANCE;
    mutationRates["node"] = NODE_MUTATION_CHANCE;
    mutationRates["enable"] = ENABLE_MUTATION_CHANCE;
    mutationRates["disable"] = DISABLE_MUTATION_CHANCE;
    mutationRates["transfer"] = TRANSFER_MUTATION_CHANCE;
    mutationRates["delete"] = DELETE_MUTATION_CHANCE;
    mutationRates["step"] = STEPSIZE;

    poolPointer = poolPtr;
    if(createNeurons){
        for( unsigned int i = 0; i < poolPointer->inputs; i++){
            neurons.push_back(new neuron());
            neurons[i]->calculated = true;
        }

        for( unsigned int i = 0; i < poolPointer->outputs; i++){
            neuron* neuronPointer = new neuron();
            neuronPointer->activated = true;
            neurons.push_back(neuronPointer);
        }
    }
    precision = 1.0;
    targetPrecision = PRECISION;
    calculateScore = true;
    maxTileSeen = 0;
}

genome::genome(genome &copyGenome) {
    fitness = std::numeric_limits<double>::min();
    maxneuron = copyGenome.maxneuron;
    globalRank = 0;

    for(auto const& x : copyGenome.genes){
        gene* copyGene = new gene(*x);
        genes.push_back(copyGene);
    }

    for (auto const& x : copyGenome.mutationRates){
        mutationRates[x.first] = x.second;
    }
    //Why should be reset the stepsize?
    //mutationRates["step"] = STEPSIZE;

    poolPointer = copyGenome.poolPointer;

    for( auto const& x : copyGenome.neurons){
        neuron* neuronPointer = new neuron();
        neuronPointer->transfer = x->transfer;
        neuronPointer->bias = x->bias;
        neuronPointer->calculated = x->calculated;
        neuronPointer->activated = x->activated;
        neurons.push_back(neuronPointer);
    }

    for( auto const& gene : genes ){
        if( gene->enabled ){
            neurons[gene->out]->addIncoming(gene);
        } else{
            neurons[gene->out]->addDisabledIncoming(gene);
        }
    }
    precision = 1.0;
    targetPrecision = PRECISION;
    calculateScore = true;
    maxTileSeen = copyGenome.maxTileSeen;
}

genome* genome::basicGenome(pool *poolPtr){
    genome* outputGenome = new genome(poolPtr);
    outputGenome->mutate();
    return outputGenome;
}

genome* genome::crossover(genome* genome1, genome* genome2){
    if(genome2->fitness > genome1->fitness){
        genome *tmpGenome = genome1;
        genome1 = genome2;
        genome2 = tmpGenome;
    }

    genome *child = new genome(genome1->poolPointer, false);
    child->maxTileSeen = std::max(genome1->maxTileSeen, genome2->maxTileSeen);

    for( auto const& x : genome1->neurons){
        neuron* neuronPointer = new neuron();
        neuronPointer->transfer = x->transfer;
        neuronPointer->bias = x->bias;
        neuronPointer->calculated = x->calculated;
        neuronPointer->activated = x->activated;
        child->neurons.push_back(neuronPointer);
    }

    gene* copyGene;
    std::map<unsigned int, gene*> innovations;

    for( auto const& x : genome2->genes){
        innovations[x->innovation] = x;
    }

    for( auto const& gene1 : genome1->genes){
        auto search = innovations.find(gene1->innovation);
        if( search != innovations.end()){
            gene* gene2 = innovations[gene1->innovation];
            if(gene2->enabled && rng[0].rand() > 0.5)
                copyGene = new gene(*gene2);
            else
                copyGene = new gene(*gene1);
        } else
            copyGene = new gene(*gene1);
        if( copyGene->enabled )
            child->neurons[copyGene->out]->addIncoming(copyGene);
        else
            child->neurons[copyGene->out]->addDisabledIncoming(copyGene);
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

    std::vector<unsigned int> innovations;
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
    return disjointGenes * 1.0/ std::max(genome1->genes.size(), genome2->genes.size());
}

double genome::weights(genome* genome1, genome* genome2){
    std::map<unsigned int, gene*> innovations;
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

double genome::bias(genome *genome1, genome *genome2){
    std::vector<unsigned int> innovations;
        std::vector<unsigned int> neuronList;
    for( auto const& x : genome2->genes)
        innovations.push_back(x->innovation);

    double sum = 0.0;

    for( auto const& x : genome1->genes){
        auto search =  find(innovations.begin(), innovations.end(), x->innovation);
        if( search != innovations.end()){
            auto search2 = find(neuronList.begin(), neuronList.end(), x->out);
            if( search2 == neuronList.end()){
                sum += std::abs(genome2->neurons[x->out]->bias - genome1->neurons[x->out]->bias);
                neuronList.push_back(x->out);
            }
        }
    }
    if( neuronList.size() > 0)
        return sum / neuronList.size();
    return 0.0;
}

double genome::transfer(genome *genome1, genome *genome2){
    std::vector<unsigned int> innovations;
    std::vector<unsigned int> neuronList;
    for( auto const& x : genome2->genes)
        innovations.push_back(x->innovation);

    unsigned int disjoint = 0;
    unsigned int sum = 0;

    for( auto const& x : genome1->genes){
        auto search =  find(innovations.begin(), innovations.end(), x->innovation);
        if( search != innovations.end()){
            auto search2 = find(neuronList.begin(), neuronList.end(), x->out);
            if( search2 == neuronList.end()){
                if(genome2->neurons[x->out]->transfer != genome1->neurons[x->out]->transfer)
                    disjoint++;
                sum++;
            }
        }
    }
    if(sum)
        return disjoint*1.0/sum;
    return 0.0;
}

double genome::rates(genome *genome1, genome *genome2){
    double diff = 0.0;
    for(auto const& mutation : genome1->mutationRates){
        diff += std::abs(genome1->mutationRates[mutation.first] - genome2->mutationRates[mutation.first]);
    }
    return diff/genome1->mutationRates.size();
}

bool genome::sameSpecies(genome* genome1, genome* genome2){
    double dd = DELTA_DISJOINT * disjoint(genome1, genome2);
    double dw = DELTA_WEIGHTS * weights(genome1, genome2);
    double db = DELTA_BIAS * bias(genome1, genome2);
    double dt = DELTA_TRANSFER * transfer(genome1, genome2);
    double dr = DELTA_RATES * rates(genome1, genome2);

    return( dd + dw + db + dt < DELTA_THRESHOLD);
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

void genome::evaluate(uint64_t board, unsigned char maxTile, std::vector<double> &output){
    for(std::vector<neuron*>::iterator it = neurons.begin()+poolPointer->inputs; it != neurons.end(); ++it)
        (*it)->calculated = false;

    if(maxTileSeen < maxTile)
        maxTileSeen = maxTile;

    for(unsigned short i = 0; i < 16*(maxTileSeen+1); ++i){
        neuron *neuronPointer = neurons[i];

        if(((board >> 4*(i%16)) & 0x000FULL) == i/16){
            neuronPointer->value = 1.0;
            neuronPointer->activated = true;
        } else
            neuronPointer->value = 0.0;
    }

    for(unsigned short i = 0; i < poolPointer->outputs; ++i){
        output[i] = calculateNeuron(poolPointer->inputs + i);
    }
}

bool genome::containsGene(gene* inputGene){
    for(auto const& x : genes){
        if(x->into == inputGene->into && x->out == inputGene->out)
            return true;
    }
    return false;
}

unsigned short genome::randomNeuron(bool includeInput){
    std::vector<unsigned short> possible;
    for(unsigned short i = 0; i < neurons.size(); i++){
        if(includeInput || !isInputNeuron(i))
            if(neurons[i]->activated)
                possible.push_back(i);
    }
    if(possible.size() > 0){
        unsigned short pick = (int)(rng[0].rand() * possible.size());
        return possible[pick];
    }
    //HACK: in first generation, linkMutate will try 0->0 every time...
    return 0;
}

bool genome::isInputNeuron(unsigned short neuronNumber){
    return(neuronNumber < poolPointer->inputs);
}

bool genome::isOutputNeuron(unsigned short neuronNumber){
    return(neuronNumber >= poolPointer->inputs && neuronNumber < poolPointer->inputs + poolPointer->outputs);
}


bool genome::areConnected(unsigned short neuron1, unsigned short neuron2){
    if(isOutputNeuron(neuron1))
        return false;
    if(isInputNeuron(neuron2))
        return false;

    if(neurons[neuron2]->checkedConnection){
        return neurons[neuron2]->connected;
    }

    neurons[neuron2]->checkedConnection = true;

    for(auto const& tmp : neurons[neuron2]->incoming){
        if(tmp->into == neuron1){
            neurons[neuron2]->connected = true;
            return true;
        }
        if(areConnected(neuron1, tmp->into)){
            neurons[neuron2]->connected = true;
            return true;
        }
    }
    for(auto const& tmp : neurons[neuron2]->disabledIncoming){
        if(tmp->into == neuron1){
            neurons[neuron2]->connected = true;
            return true;
        }
        if(areConnected(neuron1, tmp->into)){
            neurons[neuron2]->connected = true;
            return true;
        }
    }
    neurons[neuron2]->connected = false;
    return false;
}

bool genome::linkAllowed(unsigned short neuron1, unsigned short neuron2){
    if(isInputNeuron(neuron2))
        return false;
    if(isOutputNeuron(neuron1))
        return false;
    if(neuron1 == neuron2)
        return false;
    if(!neurons[neuron1]->activated)
        return false;

    for(auto const& tmp : neurons)
        tmp->checkedConnection = false;
    if(areConnected(neuron2, neuron1))
        return false;
    return true;
}

void genome::mutate(){
    /*unsigned int disabledGenes = 0;
    unsigned int enabledGenes;
    for(auto const& gen: genes){
        if(!gen->enabled){
            disabledGenes++;
        }
    }
    enabledGenes = genes.size() - disabledGenes;
    printf("Genes: %d %d %d\n", genes.size(), enabledGenes, disabledGenes);
    unsigned int mutableNeurons = neurons.size() - poolPointer->inputs;
    unsigned int activeNeurons = 0;
    for(auto const& neur: neurons){
        if(neur.second->activated)
            activeNeurons++;
    }
    printf("Neurons: %d %d, %d\n", neurons.size(), activeNeurons, mutableNeurons);
    printf("Innovation: %d\n", poolPointer->innovation);*/

    for(auto const& mutation : mutationRates){
        if(rng[0].rand()> 0.5)
            mutationRates[mutation.first] *= 0.95;
        else
            mutationRates[mutation.first] /= 0.95;
        if(!mutation.first.compare("step"))
            continue;
        double p = mutation.second;
        //printf("%s %f\n", mutation.first.c_str(), mutation.second);

        /*if(!mutation.first.compare("weight"))
            p *= enabledGenes;
        else if(!mutation.first.compare("link"))
            p *= activeNeurons;
        else if(!mutation.first.compare("bias"))
            p *= mutableNeurons;
        else if(!mutation.first.compare("node"))
            p *= enabledGenes;
        else if(!mutation.first.compare("enable"))
            p *= disabledGenes;
        else if(!mutation.first.compare("disable"))
            p *= enabledGenes;
        else if(!mutation.first.compare("transfer"))
            p *= mutableNeurons;
        else if(!mutation.first.compare("delete"))
            p *= disabledGenes;*/

        while(p > 0){
            if(rng[0].rand() < p){
                if(!mutation.first.compare("weight"))
                    weightMutate();
                else if(!mutation.first.compare("link"))
                    linkMutate();
                else if(!mutation.first.compare("bias"))
                    biasMutate();
                else if(!mutation.first.compare("node"))
                    nodeMutate();
                else if(!mutation.first.compare("enable"))
                    disableToEnableMutate();
                else if(!mutation.first.compare("disable"))
                    enableToDisableMutate();
                else if(!mutation.first.compare("transfer"))
                    transferMutate();
                else if(!mutation.first.compare("delete"))
                    deleteDisabledMutate();
            }
            p -= 1.0;
        }
    }
}

void genome::weightMutate(){
    if(!genes.size())
        return;
    gene* pick = genes[(int)(rng[0].rand()*genes.size())];
    if(!pick->enabled)
        return;
    if(rng[0].rand() < PERTUBCHANCE)
        pick->weight += (rng[0].rand() - 0.5) * mutationRates["step"];
    else
        pick->weight = (rng[0].rand() - 0.5) * WEIGHT_RANGE;
}

void genome::linkMutate(){
    unsigned short neuron1 = randomNeuron(true);
    unsigned short neuron2 = randomNeuron(false);

    if(!linkAllowed(neuron1, neuron2)){
        if(linkAllowed(neuron2, neuron1)){
            unsigned short tmp = neuron1;
            neuron1 = neuron2;
            neuron2 = tmp;
        } else {
            return;
        }
    }

    gene* newGene = new gene();
    newGene->into = neuron1;
    newGene->out = neuron2;

    if(containsGene(newGene)){
        delete newGene;
        return;
    }

    newGene->innovation = poolPointer->newInnovation();
    newGene->weight = (rng[0].rand() - 0.5)*WEIGHT_RANGE;
    genes.push_back(newGene);
    neurons[neuron2]->addIncoming(newGene);
}

void genome::biasMutate(){
    unsigned short pick = randomNeuron(false);
    if(rng[0].rand() < PERTUBCHANCE){
        neurons[pick]->bias += (rng[0].rand() - 0.5) * mutationRates["step"];
    } else {
        neurons[pick]->bias = (rng[0].rand() - 0.5) * BIAS_RANGE;
    }
}

void genome::nodeMutate(){
    if(!genes.size())
        return;

    unsigned short randomIndex = (int)(rng[0].rand()*genes.size());
    if(!genes[randomIndex]->enabled)
        return;

    gene* oldGene = genes[randomIndex];
    neurons[oldGene->out]->disableIncoming(oldGene);
    oldGene->enabled = false;

    maxneuron += 1;

    gene* gene1 = new gene();
    gene1->out = maxneuron;
    gene1->into = oldGene->into;
    gene1->weight = 1.0;
    gene1->innovation = poolPointer->newInnovation();
    genes.push_back(gene1);
    neuron* neuronPointer = new neuron();
    neuronPointer->transfer = &neuron::id;
    neuronPointer->addIncoming(gene1);
    neurons.push_back(neuronPointer);

    gene* gene2 = new gene();
    gene2->out = oldGene->out;
    gene2->into = maxneuron;
    gene2->weight = oldGene->weight;
    gene2->innovation = poolPointer->newInnovation();
    genes.push_back(gene2);
    neurons[gene2->out]->addIncoming(gene2);
}

void genome::enableToDisableMutate(){
    std::vector<gene*> candidates;
    for(auto const& x : genes){
        if(x->enabled)
            candidates.push_back(x);
    }

    if(!candidates.size())
        return;

    gene* pick = candidates[(int)(rng[0].rand()*candidates.size())];
    pick->enabled = false;
    neurons[pick->out]->disableIncoming(pick);
}

void genome::disableToEnableMutate(){
    std::vector<gene*> candidates;
    for(auto const& x : genes){
        if(!x->enabled)
            candidates.push_back(x);
    }

    if(!candidates.size())
        return;

    gene* pick = candidates[(int)(rng[0].rand()*candidates.size())];
    pick->enabled = true;
    neurons[pick->out]->enableIncoming(pick);
}

void genome::transferMutate(){
    unsigned short randomIndex = randomNeuron(false);
    double pick = rng[0].rand();
    if(pick < 0.25)
        neurons[randomIndex]->transfer = &neuron::sigmoid;
    else if(pick < 0.5)
        neurons[randomIndex]->transfer = &neuron::step;
    else if(pick < 0.75)
        neurons[randomIndex]->transfer = &neuron::slope;
    else
        neurons[randomIndex]->transfer = &neuron::id;
}

void genome::deleteDisabledMutate(){
    std::vector<gene*> candidates;
    for(auto const& gen: genes){
        if(!gen->enabled){
            candidates.push_back(gen);
        }
    }
    if(!candidates.size())
        return;

    gene* pick = candidates[(int)(rng[0].rand()*candidates.size())];
    std::vector<gene*>::iterator it = find (genes.begin(), genes.end(), pick);;
    if (it != genes.end()) {
        genes.erase(it);
        neurons[pick->out]->removeDisabledIncoming(pick);
    }
    return;
}

void genome::calculateFitness(){
    double sum = 0.0;
    for(auto const& score : scores)
        sum += score;
    double mean = sum/scores.size();
    double variance = 0.0;
    for(auto const& score : scores){
        variance += pow((sum-score)/(scores.size()-1)-mean,2);
    }
    variance *= (scores.size()-1.0)/scores.size();
    precision = sqrt(variance)/mean;
    fitness = mean;
}
