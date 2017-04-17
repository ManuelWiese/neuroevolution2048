#include "gene.h"

gene::gene(){
    into = 0;
    out = 0;
    weight = 0.0;
    enabled = true;
    innovation = 0;
}

gene::gene(gene &copyGene){
    into = copyGene.into;
    out = copyGene.out;
    weight = copyGene.weight;
    enabled = copyGene.enabled;
    innovation = copyGene.innovation;
}

void gene::print(){
    printf("into = %d\nout = %d\nweight = %f\nenabled = %d\ninnovation = %d\n", into, out, weight, enabled, innovation);
}

std::ostream& operator<<(std::ostream& os, const gene& g) {
    os << g.into << std::endl;
    os << g.out << std::endl;
    os << g.weight << std::endl;
    os << g.enabled << std::endl;
    os << g.innovation << std::endl;

    return os;
}

std::istream& operator>>(std::istream& is, gene& g){
    is >> g.into;
    is >> g.out;
    is >> g.weight;
    is >> g.enabled;
    is >> g.innovation;

    return is;
}
