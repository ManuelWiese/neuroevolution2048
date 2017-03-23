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
