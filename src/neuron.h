#ifndef NEURON_H
#define NEURON_H

#include "gene.h"
#include <vector>
#include <array>
#include <iostream>


class neuron{
public:
    std::vector<gene*>  incoming;
    std::vector<gene*>  disabledIncoming;
    std::vector<gene*>  outgoing;
    
    double              value;
    double              bias;
    bool                calculated;

    bool                checkedConnection;
    bool                connected;

    double              delta;

                        neuron();

    double              (*transfer)(double);

    static std::array<double, 201> sigmoidArray;
    static double       sigmoid(double x);
    static double       dSigmoidInvSigmoid(double y);
    static double       step(double x);
    static double       slope(double x);
    static double       id(double x);
    static double       dId(double y);

    void                addIncoming(gene *geneIncoming);
    void                removeIncoming(gene *geneIncoming);
    void                addDisabledIncoming(gene* geneIncoming);
    void                removeDisabledIncoming(gene* geneIncoming);
    void                disableIncoming(gene *geneIncoming);
    void                enableIncoming(gene *geneIncoming);

    void                addOutgoing(gene *geneOutgoing);
    void                removeOutgoing(gene *geneOutgoing);

    void                print();

    friend std::ostream& operator<<(std::ostream& os, const neuron& n);
    friend std::istream& operator>>(std::istream& is, neuron& n);
};


#endif //NEURON_H
