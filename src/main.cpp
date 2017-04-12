#include <random>
#include <cmath>
#include "game.h"

std::random_device rd;
std::mt19937 generator(rd());
std::uniform_real_distribution<> distribution(0, 1);


int main(){
    game_t game;
    game.learn();
    return 0;
}
