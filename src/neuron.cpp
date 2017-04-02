#include "neuron.h"
#include <cmath>
#include <algorithm>

neuron::neuron(){
    value = 0.0;
    depth = -1;
    calculated = false;
    bias = 0.0;
    activated = false;
    transfer = &sigmoid;
}

double neuron::sigmoid(double x){
    double output = 1.0 / (1.0 + exp(-1.0 * x));
    if (output == HUGE_VAL)
        return 1.0;
    if (output == -HUGE_VAL)
        return 0.0;
    return output;
}

double neuron::step(double x){
    if (x > 0)
        return 1.0;
    return 0.0;
}

double neuron::slope(double x){
    if (x >= 0.5)
        return 1.0;
    if (x <= -0.5)
        return 0.0;
    return x;
}

double neuron::id(double x){
    return x;
}

void neuron::addIncoming(gene *geneIncoming){
    incoming.push_back(geneIncoming);
}

void neuron::removeIncoming(gene *geneIncoming){
    std::vector<gene*>::iterator it;
    it = find (incoming.begin(), incoming.end(), geneIncoming);
    if (it != incoming.end()) {
        incoming.erase(it);
        return;
    }
    printf("Cannot remove gene, gene not found\n");
    return;
}

void neuron::addDisabledIncoming(gene *geneIncoming){
    disabledIncoming.push_back(geneIncoming);
}

void neuron::removeDisabledIncoming(gene *geneIncoming){
    std::vector<gene*>::iterator it;
    it = find (disabledIncoming.begin(), disabledIncoming.end(), geneIncoming);
    if (it != disabledIncoming.end()) {
        disabledIncoming.erase(it);
        return;
    }
    printf("Cannot remove gene, gene not found\n");
    return;
}

void neuron::disableIncoming(gene *geneIncoming){
    removeIncoming(geneIncoming);
    disabledIncoming.push_back(geneIncoming);
}

void neuron::enableIncoming(gene *geneIncoming){
    std::vector<gene*>::iterator it = find (disabledIncoming.begin(), disabledIncoming.end(), geneIncoming);
    if(it != disabledIncoming.end()){
        disabledIncoming.erase(it);
        addIncoming(geneIncoming);
        return;
    }
    printf("Cannot remove gene, gene not found\n");
    return;
}

void neuron::print(){
    for ( auto &i : incoming ) {
        i->print();
    }
}
