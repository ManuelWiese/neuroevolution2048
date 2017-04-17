#ifndef GENE_H
#define GENE_H

#include <cstdio>
#include <iostream>

class gene{
public:
    unsigned short  into;
    unsigned short  out;
    double          weight;
    bool            enabled;
    unsigned int    innovation;

    gene();
    gene(gene &copyGene);
    void print();

    friend std::ostream& operator<<(std::ostream& os, const gene& g);
    friend std::istream& operator>>(std::istream& is, gene& g);
};

#endif //GENE_H
