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
    double              value;
    double              bias;
    bool                calculated;

    bool                checkedConnection;
    bool                connected;

                        neuron();

    double              (*transfer)(double);

    static std::array<double, 201> sigmoidArray;
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

    friend std::ostream& operator<<(std::ostream& os, const neuron& n);
    friend std::istream& operator>>(std::istream& is, neuron& n);
};


#endif //NEURON_H
