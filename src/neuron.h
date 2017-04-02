#ifndef NEURON_H
#define NEURON_H

#include "gene.h"
#include <vector>


class neuron{
public:
    std::vector<gene*>  incoming;
    std::vector<gene*>  disabledIncoming;
    double              value;
    short               depth;
    double              bias;
    bool                calculated;
    bool                activated;
    
    bool                checkedConnection;
    bool                connected;

                        neuron();

    double              (*transfer)(double);

    static double       sigmoid(double x);
    static double       step(double x);
    static double       slope(double x);
    static double       id(double x);

    void                addIncoming(gene *geneIncoming);
    void                removeIncoming(gene *geneIncoming);
    void                addDisabledIncoming(gene* geneIncoming);
    void                removeDisabledIncoming(gene* geneIncoming);
    void                disableIncoming(gene *geneIncoming);
    void                enableIncoming(gene *geneIncoming);

    void                print();
};


#endif //NEURON_H
