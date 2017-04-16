#include "neuron.h"
#include <cmath>
#include <algorithm>

neuron::neuron(){
    value = 0.0;
    depth = -1;
    calculated = false;
    bias = 0.0;
    transfer = &sigmoid;
}

std::array<double, 201> neuron::sigmoidArray = {0.000045, 0.000050, 0.000055, 0.000061, 0.000068, 0.000075,
                                                0.000083, 0.000091, 0.000101, 0.000112, 0.000123, 0.000136,
                                                0.000151, 0.000167, 0.000184, 0.000203, 0.000225, 0.000248,
                                                0.000275, 0.000303, 0.000335, 0.000371, 0.000410, 0.000453,
                                                0.000500, 0.000553, 0.000611, 0.000675, 0.000746, 0.000824,
                                                0.000911, 0.001007, 0.001113, 0.001229, 0.001359, 0.001501,
                                                0.001659, 0.001833, 0.002025, 0.002238, 0.002473, 0.002732,
                                                0.003018, 0.003335, 0.003684, 0.004070, 0.004496, 0.004967,
                                                0.005486, 0.006060, 0.006693, 0.007392, 0.008163, 0.009013,
                                                0.009952, 0.010987, 0.012128, 0.013387, 0.014774, 0.016302,
                                                0.017986, 0.019840, 0.021881, 0.024127, 0.026597, 0.029312,
                                                0.032295, 0.035571, 0.039166, 0.043107, 0.047426, 0.052154,
                                                0.057324, 0.062973, 0.069138, 0.075858, 0.083173, 0.091123,
                                                0.099750, 0.109097, 0.119203, 0.130108, 0.141851, 0.154465,
                                                0.167982, 0.182426, 0.197816, 0.214165, 0.231475, 0.249740,
                                                0.268941, 0.289050, 0.310026, 0.331812, 0.354344, 0.377541,
                                                0.401312, 0.425557, 0.450166, 0.475021, 0.500000, 0.524979,
                                                0.549834, 0.574443, 0.598688, 0.622459, 0.645656, 0.668188,
                                                0.689974, 0.710950, 0.731059, 0.750260, 0.768525, 0.785835,
                                                0.802184, 0.817574, 0.832018, 0.845535, 0.858149, 0.869892,
                                                0.880797, 0.890903, 0.900250, 0.908877, 0.916827, 0.924142,
                                                0.930862, 0.937027, 0.942676, 0.947846, 0.952574, 0.956893,
                                                0.960834, 0.964429, 0.967705, 0.970688, 0.973403, 0.975873,
                                                0.978119, 0.980160, 0.982014, 0.983698, 0.985226, 0.986613,
                                                0.987872, 0.989013, 0.990048, 0.990987, 0.991837, 0.992608,
                                                0.993307, 0.993940, 0.994514, 0.995033, 0.995504, 0.995930,
                                                0.996316, 0.996665, 0.996982, 0.997268, 0.997527, 0.997762,
                                                0.997975, 0.998167, 0.998341, 0.998499, 0.998641, 0.998771,
                                                0.998887, 0.998993, 0.999089, 0.999176, 0.999254, 0.999325,
                                                0.999389, 0.999447, 0.999500, 0.999547, 0.999590, 0.999629,
                                                0.999665, 0.999697, 0.999725, 0.999752, 0.999775, 0.999797,
                                                0.999816, 0.999833, 0.999849, 0.999864, 0.999877, 0.999888,
                                                0.999899, 0.999909, 0.999917, 0.999925, 0.999932, 0.999939,
                                                0.999945, 0.999950, 0.999955};

double neuron::sigmoid(double x){
    if(x < -10.0)
        return 0.0;
    if(x > 10.0)
        return 1.0;
    unsigned char index = (x+10)/0.1;
    double delta = (sigmoidArray[index+1] - sigmoidArray[index])/0.1;
    return sigmoidArray[index] + delta*(x-(index)*0.1+10);
}

/*double neuron::sigmoid(double x){
	return 1.0/(1.0+exp(-x));
}*/

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
