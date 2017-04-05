#include "game.h"
#include "pool.h"
#include <fstream>

game::game() {
    for( unsigned char i = 0; i < field.size(); i++ ) {
        field[i] = 0;
    }
    for(unsigned char i = 0; i <= N; i++){
        powersOf16[i] = pow(16, i);
    }
    emptyCells = N*N-2;
    score = 0;
    spawnNumber();
    spawnNumber();

    fillPushList();
}

bool game::spawnNumber() {
    std::vector<unsigned char> emptyList;
    for(unsigned char index = 0; index < N*N; ++index){
        if(!getCell(index))
            emptyList.push_back(index);
    }
    if(!emptyList.size())
        return false;

    unsigned char spawn = dis(gen) > 0.9 ? 2 : 1;
    unsigned char pick = (unsigned char)(dis(gen) * emptyList.size());
    setCell(emptyList[pick], spawn);
    return true;
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
    return getCell(index%N, index/N);
}

//TODO: faster with just - and + operation using getCell?
void game::setCell(unsigned char i, unsigned char j, unsigned char value) {
    unsigned int mask = 0;
    for( unsigned char count = 0; count < field.size(); count++ ) {
        if( count != i ) {
            mask += 15*powersOf16[count];
        }
    }
    field[j] &= mask;
    field[j] += value*powersOf16[i];
}

void game::setCell(unsigned char index, unsigned char value){
    setCell(index%N, index/N, value);
}

std::vector<unsigned char> game::getHistogram(){
    std::vector<unsigned char> output(16,0);
    for( unsigned char i = 0; i < N; i++) {
        for( unsigned char j = 0; j < N; j++) {
            output[getCell(i,j)]++;
        }
    }
    return output;
}

unsigned char game::getMaxTile(){
    unsigned char max = 0;
    for(unsigned char i = 0; i < N; i++){
        for(unsigned char j = 0; j < N; j++){
            unsigned char cell = getCell(i,j);
            if(max < cell)
                max = cell;
        }
    }
    return max;
}

void game::fillPushList() {
    for( unsigned int row = 0; row < powersOf16[N]; row++ ) {
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
            tmpRow += rowArray[i]*powersOf16[i];
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
            output[i] += ((field[j] >> 4*i) & 15 ) * powersOf16[j];
        }
    }
    return output;
}

unsigned int game::reverseNumber(unsigned int number) {
    unsigned int output = 0;
    for( unsigned char i = 0; i < N; i++ ) {
        output += (( number >> 4*i ) & 15 )*powersOf16[N-i-1];
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

void game::fieldToInput(std::vector<double> &input){
    //HACK: input will be reset to 0.0 in genome::evaluate
    //std::fill(input.begin(), input.end(), 0.0);
    for(unsigned char i = 0; i < N*N; i++){
        input[getCell(i)*16 + i] = 1.0;
    }
}

std::vector<double> game::fieldToFlatField(){
    std::vector<double> input;
    for(unsigned char j = 0; j < N; j++){
        for(unsigned char k = 0; k < N; k++){
            input.push_back(getCell(j,k));
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

static double getMedian(std::vector<double> &input){
    std::sort(input.begin(), input.end());
    if(input.size() % 2){
        return input[(input.size()-1)/2];
    }
    return (input[input.size()/2]+input[input.size()/2-1])/2.0;
}

static std::vector<double> getTileProbability(std::vector<double> &maxTile){
    std::vector<double> result(16, 0);
    for(auto const &tile : maxTile){
        for(unsigned short i=0; i<=tile; i++){
            result[i]+=1.0;
        }
    }
    for(unsigned short i=0; i<result.size(); i++){
        result[i] /= maxTile.size();
    }
    return result;
}

static void writeGenerationScore(std::vector<double> &generationScore, pool &mainPool){
    std::ofstream generationFile;
    generationFile.open(mainPool.timestamp + "_generation.dat", std::ofstream::out | std::ofstream::app);
    double generationMean = 0.0;

    for(auto const& x : generationScore){
        generationMean += x;
    }
    generationMean = generationMean / generationScore.size();
    generationFile << mainPool.generation - 1 << "  " << generationMean << std::endl;
    generationScore.erase(generationScore.begin(), generationScore.end());

    generationFile.close();
}

static void writeTileProbability(std::vector<double> &maxTileVector, pool &mainPool){
    std::ofstream tileProbabilityFile;
    tileProbabilityFile.open(mainPool.timestamp + "_tileprobability.dat", std::ofstream::out | std::ofstream::app);

    std::vector<double> tileProbability = getTileProbability(maxTileVector);
    tileProbabilityFile << mainPool.generation - 1 << " ";
    for(auto const& prob: tileProbability)
        tileProbabilityFile << prob << "    ";
    tileProbabilityFile << std::endl;
    maxTileVector.erase(maxTileVector.begin(), maxTileVector.end());

    tileProbabilityFile.close();
}

static void writeMutationRates(pool &mainPool){
    std::ofstream mutationRatesFile;
    mutationRatesFile.open(mainPool.timestamp + "_mutationrates.dat", std::ofstream::out | std::ofstream::app);
    std::vector<double> ratesMean(9, 0);
    std::vector<double> ratesSecondMoment(9, 0);
    for(auto const &spec : mainPool.speciesVector){
        for(auto const &genom : spec->genomes){
            ratesMean[0] += genom->mutationRates["weight"];
            ratesMean[1] += genom->mutationRates["link"];
            ratesMean[2] += genom->mutationRates["bias"];
            ratesMean[3] += genom->mutationRates["node"];
            ratesMean[4] += genom->mutationRates["enable"];
            ratesMean[5] += genom->mutationRates["disable"];
            ratesMean[6] += genom->mutationRates["transfer"];
            ratesMean[7] += genom->mutationRates["delete"];
            ratesMean[8] += genom->mutationRates["step"];

            ratesSecondMoment[0] += pow(genom->mutationRates["weight"], 2);
            ratesSecondMoment[1] += pow(genom->mutationRates["link"], 2);
            ratesSecondMoment[2] += pow(genom->mutationRates["bias"], 2);
            ratesSecondMoment[3] += pow(genom->mutationRates["node"], 2);
            ratesSecondMoment[4] += pow(genom->mutationRates["enable"], 2);
            ratesSecondMoment[5] += pow(genom->mutationRates["disable"], 2);
            ratesSecondMoment[6] += pow(genom->mutationRates["transfer"], 2);
            ratesSecondMoment[7] += pow(genom->mutationRates["delete"], 2);
            ratesSecondMoment[8] += pow(genom->mutationRates["step"], 2);
        }
    }

    mutationRatesFile << mainPool.generation << " ";
    for(unsigned char i=0; i<ratesMean.size(); i++)
        mutationRatesFile << ratesMean[i] / mainPool.population << "    " << sqrt(ratesSecondMoment[i] / mainPool.population - pow(ratesMean[i] / mainPool.population, 2)) << "    ";
    mutationRatesFile << std::endl;

    mutationRatesFile.close();
}

static void writeStats(pool &mainPool){
    std::ofstream statsFile;
    statsFile.open(mainPool.timestamp + "_stats.dat", std::ofstream::out | std::ofstream::app);

    unsigned int neurons = 0, activeNeurons = 0, mutableNeurons = 0;
    unsigned int genes = 0, enabledGenes = 0, disabledGenes = 0;

    for(auto const& spec : mainPool.speciesVector){
        for(auto const& genom : spec->genomes){
            genes += genom->genes.size();
            for(auto const& gen : genom->genes){
                if(gen->enabled)
                    enabledGenes++;
                else
                    disabledGenes++;
            }
            neurons += genom->neurons.size();
            mutableNeurons += genom->neurons.size() - mainPool.inputs;
            for(auto const& neur : genom->neurons){
                if(neur->activated)
                    activeNeurons++;
            }
        }
    }

    statsFile << mainPool.generation << "   " << neurons*1.0/mainPool.population << "   " << activeNeurons*1.0/mainPool.population << " " << mutableNeurons*1.0/mainPool.population << "    "
              << genes*1.0/mainPool.population << " " << enabledGenes*1.0/mainPool.population << "  " << disabledGenes*1.0/mainPool.population << " " << mainPool.speciesVector.size() << std::endl;

    statsFile.close();
}

void game::autoSolve() {
    unsigned short runsPerNetwork = 10;
    unsigned short generations = 150;
    std::array<unsigned int, N> oldField = field;
    unsigned int oldScore = score;

    pool mainPool(N*N*16, 4, POPULATION);
    std::vector<double> input(N*N*16, 0.0);
    std::ofstream scoreFile;
    scoreFile.open(mainPool.timestamp + ".dat", std::ofstream::out | std::ofstream::app);
    std::vector<double> generationScore;
    std::vector<double> maxTileVector;

    while(true){
        double meanScore = 0.0;
        genome* currentGenome = mainPool.speciesVector[mainPool.currentSpecies]->genomes[mainPool.currentGenome];
        std::vector<double> genomeScores;
        for(unsigned short run = 0; run < runsPerNetwork; run++){
            field = oldField;
            score = oldScore;
            while(true){
                fieldToInput(input);
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
            meanScore += score;//getMaxTile();
            scoreFile << score << std::endl;
            generationScore.push_back(score);
            genomeScores.push_back(score);
            maxTileVector.push_back(getMaxTile());
        }
        currentGenome->fitness = meanScore / runsPerNetwork;
        if(currentGenome->fitness > mainPool.maxFitness)
            mainPool.maxFitness = currentGenome->fitness;
        mainPool.nextGenome();

        if(mainPool.currentGenome == 0 && mainPool.currentSpecies == 0){
            writeGenerationScore(generationScore, mainPool);
            writeTileProbability(maxTileVector, mainPool);
            writeMutationRates(mainPool);
            writeStats(mainPool);
        }
    }
}
