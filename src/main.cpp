#include <random>

#include "neuron.h"
#include "gene.h"
#include "genome.h"
#include "pool.h"
#include "species.h"
#include "game.h"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0, 1);

int main() {
    game gameInstance;
    gameInstance.autoSolve();
}
