#include <random>
#include <cmath>
#include "game.h"
#include "random_t.h"
#include <omp.h>

std::random_device rd;
std::mt19937 generator(rd());
std::uniform_real_distribution<> distribution(0, 1);

std::vector<random_t> rng;

int main(){
    unsigned short threads;
    #pragma omp parallel
    {
        #pragma omp single
        {
            threads = omp_get_num_threads();
        }
    }
    for(unsigned short i = 0; i < threads; ++i)
        rng.push_back(random_t());
        
    game_t game;
    game.learn();
    return 0;
}
