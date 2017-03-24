#ifndef NEURON_H
#define NEURON_H

#include "gene.h"
#include <vector>


class neuron{
public:
    std::vector<gene*>  incoming;
    double              value;
    short               depth;
    double              bias;
    bool                calculated;
    bool                activated;

                        neuron();

    double              (*transfer)(double);

    static double       sigmoid(double x);
    static double       step(double x);
    static double       slope(double x);
    static double       id(double x);

    void                addIncoming(gene *geneIncoming);
    bool                removeIncoming(gene *geneIncoming);

    void                print();
};


#endif //NEURON_H
