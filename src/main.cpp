#include "neuron.h"
#include "gene.h"
#include "genome.h"
#include "pool.h"

int main() {
    gene gene1;
    gene1.into = 10;
    gene gene2(gene1);
    gene2.out = 20;
    gene1.print();
    gene2.print();

    neuron neuron1;
    printf("%f\n", neuron1.transfer(1));
    neuron1.transfer = &neuron::slope;
    printf("%f\n", neuron1.transfer(1));

    neuron1.addIncoming(&gene1);
    neuron1.addIncoming(&gene2);

    neuron1.print();
    gene1.into = 33;
    neuron1.print();
    neuron1.removeIncoming(&gene1);
    neuron1.print();
    neuron1.removeIncoming(&gene1);
}
