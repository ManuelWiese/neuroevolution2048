#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <array>
#include <vector>
#include <cmath>
#include <random>

extern std::mt19937 gen;
extern std::uniform_real_distribution<> dis;

const unsigned char N = 4;

class game {
public:
    std::array<unsigned int, N> field;
    unsigned char emptyCells;
    unsigned int score;

    std::vector<unsigned int> pushList;
    std::vector<unsigned int> scoreList;
    std::vector<unsigned int> emptyList;
    std::vector<int> monotonousList;
    std::array<unsigned int, N+1> powersOf16;

    game();
    void print(bool printField);
    unsigned char getCell(unsigned char i, unsigned char j);
    unsigned char getCell(unsigned char index);
    void setCell(unsigned char i, unsigned char j, unsigned char value);
    unsigned char getMaxTile();
    std::vector<unsigned char> getHistogram();
    bool moveUp();
    bool moveLeft();
    bool moveDown();
    bool moveRight();
    bool move(unsigned char direction);
    void fieldToInput(std::vector<double> &input);
    std::vector<double> fieldToFlatField();
    std::vector<unsigned char> sortOutput(std::vector<double> output);
    void autoSolve();

    void fillPushList();
    std::array<unsigned int, N> columns(std::array<unsigned int, N> field);
    unsigned int reverseNumber(unsigned int number);
    bool spawnNumber();

};

#endif // GAME_H
