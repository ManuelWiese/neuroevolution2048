#ifndef GENE_H
#define GENE_H

#include <cstdio>

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
};

#endif //GENE_H
