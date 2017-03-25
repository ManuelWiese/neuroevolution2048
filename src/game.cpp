#include "game.h"
#include "pool.h"
#include <fstream>

game::game() {
    for( unsigned char i = 0; i < field.size(); i++ ) {
        field[i] = 0;
    }
    emptyCells = N*N-2;
    score = 0;
    spawnNumber();
    spawnNumber();

    fillPushList();
}

bool game::spawnNumber() {
    emptyCells = 0;
    for( unsigned int i = 0; i < N; i++)
        for( unsigned int j = 0; j < N; j++)
            if(getCell(i, j) == 0)
                emptyCells += 1;
    if( emptyCells == 0)
        return false;

    unsigned int spawn = dis(gen) > 0.9 ? 2 : 1;
    while(true) {
        unsigned char i = floor(dis(gen) * N);
        unsigned char j = floor(dis(gen) * N);
        if( getCell(i, j) == 0 ) {
            setCell(i, j, spawn);
            return true;
        }
    }
}

void game::print(bool printField = true) {
    std::cout << score << std::endl;
    if( printField) {
        for( unsigned char i = 0; i < N; i++ ) {
            for( unsigned char j = 0; j < N; j++ ) {
                std::cout << (int)getCell(i,j) << "  ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

unsigned char game::getCell(unsigned char i, unsigned char j) {
    return((field[j] >> 4*i) & 15);
}

unsigned char game::getCell(unsigned char index) {
    unsigned int j = index / N;
    if( j % 2 == 0)
        return getCell( index % N, j);
    else
        return getCell(N - 1 - index % N, j);
}

//TODO: faster with just - and + operation using getCell?
void game::setCell(unsigned char i, unsigned char j, unsigned char value) {
    unsigned int mask = 0;
    for( unsigned char count = 0; count < field.size(); count++ ) {
        if( count != i ) {
            mask += 15*pow(16, count);
        }
    }
    field[j] &= mask;
    field[j] += value*pow(16,i);
}

std::vector<unsigned char> game::getHistogram() {
    std::vector<unsigned char> output(16,0);
    for( unsigned char i = 0; i < N; i++) {
        for( unsigned char j = 0; j < N; j++) {
            output[getCell(i,j)]++;
        }
    }
    return output;
}

void game::fillPushList() {
    for( unsigned int row = 0; row < pow(16,N); row++ ) {
        bool merged = false;
        unsigned int pushScore = 0;
        std::array<unsigned char, N> rowArray;

        unsigned char emptyCells = 0;
        bool monoUp = true;
        bool monoDown = true;

        for( unsigned char i = 0; i < N; i++) {
            rowArray[i] = (row >> i*4) & 15;
            if( rowArray[i] == 0)
                emptyCells++;
            if( i != 0 && rowArray[i] >= rowArray[i-1]) {
                monoDown = false;
            } else if( i != 0 && rowArray[i] <= rowArray[i-1]) {
                monoUp = false;
            }

        }
        //std::cout << row << "   " << (int)emptyCells << "    " << (int)monotonous << std::endl;
        emptyList.push_back(emptyCells);
        if( monoUp && monoDown )
            monotonousList.push_back(0);
        else if( monoUp )
            monotonousList.push_back(1);
        else if( monoDown )
                monotonousList.push_back(-1);
        else
            monotonousList.push_back(0);

        //std::cout << monotonousList.size() << std::endl;


        for( char i = N-1; i >= 0; i--) {
            if( rowArray[i] != 0 ) {
                for( unsigned char j = i+1; j < N; j++ ) {
                    if( rowArray[j] == 0 ) {
                        rowArray[j] = rowArray[j-1];
                        rowArray[j-1] = 0;
                        if( j == N-1 )
                            merged = false;
                    } else if( rowArray[j] == rowArray[j-1] && !merged ) {
                        rowArray[j] += 1;
                        rowArray[j-1] = 0;
                        pushScore += pow(2, rowArray[j]);
                        merged = true;
                        break;
                    } else {
                        merged = false;
                        break;
                    }
                }
            }
        }

        unsigned int tmpRow = 0;
        for( unsigned char i = 0; i < N; i++ ) {
            tmpRow += rowArray[i]*pow(16,i);
        }
        pushList.push_back(tmpRow);
        scoreList.push_back(pushScore);
    }
}

std::array<unsigned int, N> game::columns(std::array<unsigned int, N> field) {
    std::array<unsigned int, N> output;
    for( unsigned char i = 0; i < N; i++ ) {
        output[i] = 0;
        for( unsigned char j = 0; j < N; j++ ) {
            output[i] += ((field[j] >> 4*i) & 15 ) * pow(16,j);
        }
    }
    return output;
}

unsigned int game::reverseNumber(unsigned int number) {
    unsigned int output = 0;
    for( unsigned char i = 0; i < N; i++ ) {
        output += (( number >> 4*i ) & 15 )*pow(16, N-i-1);
    }
    return output;
}

bool game::moveRight() {
    bool changed = false;
    for( unsigned char i = 0; i < N; i++ ) {
        if( field[i] != pushList[field[i]] ) {
            changed = true;
            score += scoreList[field[i]];
            field[i] = pushList[field[i]];
        }
    }
    return changed;
}

bool game::moveLeft() {
    bool changed = false;
    for( unsigned char i = 0; i < N; i++ ) {
        if( field[i] != reverseNumber( pushList[reverseNumber(field[i]) ] ) ) {
            changed = true;
            score += scoreList[reverseNumber( field[i] )];
            field[i] = reverseNumber( pushList[ reverseNumber( field[i] ) ] );
        }
    }
    return changed;
}

bool game::moveUp() {
    bool changed = false;
    std::array<unsigned int, N> col = columns(field);
    for( unsigned char i = 0; i < N; i++ ) {
        if( col[i] != reverseNumber( pushList[reverseNumber( col[i])] ) ) {
            changed = true;
            score += scoreList[reverseNumber( col[i])];
            col[i] = reverseNumber( pushList[reverseNumber( col[i])]);
        }
    }
    field = columns(col);
    return changed;
}

bool game::moveDown() {
    bool changed = false;
    std::array<unsigned int, N> col = columns(field);
    for( unsigned char i = 0; i < N; i++ ) {
        if( col[i] != pushList[col[i]] ) {
            changed = true;
            score += scoreList[col[i]];
            col[i] = pushList[col[i]];
        }
    }
    field = columns(col);
    return changed;
}

bool game::move(unsigned char direction) {
    if( direction == 0)
        return moveUp();
    if( direction == 1)
        return moveLeft();
    if( direction == 2)
        return moveDown();
    if( direction == 3)
        return moveRight();
    return false;
}

std::vector<double> game::fieldToInput(){
    std::vector<double> input;
    for( unsigned char i = 0; i < 16; i++){
        for(unsigned char j = 0; j < N; j++){
            for(unsigned char k = 0; k < N; k++){
                if(getCell(j,k) == i)
                    input.push_back(1.0);
                else
                    input.push_back(0.0);
            }
        }
    }
    return input;
}

std::vector<unsigned char> game::sortOutput(std::vector<double> output){
    std::vector<unsigned char> indices(output.size());
    std::iota(begin(indices), end(indices), static_cast<size_t>(0));

    std::sort(
        begin(indices), end(indices),
        [&](size_t a, size_t b) { return output[a] > output[b]; }
    );
    return indices;
}

void game::autoSolve() {
    unsigned short runsPerNetwork = 10;
    std::array<unsigned int, N> oldField = field;
    unsigned int oldScore = score;

    pool mainPool(N*N*16, 4, POPULATION);
    std::ofstream scoreFile;
    scoreFile.open(mainPool.timestamp + ".dat", std::ofstream::out | std::ofstream::app);
    std::ofstream generationFile;
    generationFile.open(mainPool.timestamp + "_generation.dat", std::ofstream::out | std::ofstream::app);
    std::vector<double> generationScore;
    while(true){
        double meanScore = 0.0;
        genome* currentGenome = mainPool.speciesVector[mainPool.currentSpecies]->genomes[mainPool.currentGenome];
        for(unsigned short run = 0; run < runsPerNetwork; run++){
            field = oldField;
            score = oldScore;
            while(true){
                std::vector<double> input = fieldToInput();
                std::vector<double> output = currentGenome->evaluate(input);
                std::vector<unsigned char> sorted = sortOutput(output);
                for(auto const& x : sorted){
                    bool legalmove = move(x);
                    if(legalmove)
                        break;
                }
                if(!spawnNumber())
                    break;
            }
            meanScore += score;
            scoreFile << score << std::endl;
            generationScore.push_back(score);
        }
        currentGenome->fitness = meanScore / runsPerNetwork;
        if(currentGenome->fitness > mainPool.maxFitness)
            mainPool.maxFitness = currentGenome->fitness;
        mainPool.nextGenome();
        if(mainPool.currentGenome == 0 && mainPool.currentSpecies == 0){
            double generationMean = 0.0;
            for(auto const& x : generationScore){
                generationMean += x;
            }
            generationMean = generationMean / (mainPool.population * runsPerNetwork);
            generationFile << mainPool.generation - 1 << "  " << generationMean << std::endl;
            generationScore.erase(generationScore.begin(), generationScore.end());
        }
    }
}
