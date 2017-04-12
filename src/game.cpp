#include "game.h"

game_t::game_t(){
    for(unsigned int row = 0; row < 65536; ++row){
        pushRowRight[row] = row ^ pushRow(row);
        pushRowLeft[row] = row ^ reverseRow(pushRow(reverseRow(row)));
        pushColUp[row] = rowToColumn(row) ^ rowToColumn(pushRow(row));
        pushColDown[row] = rowToColumn(row) ^ rowToColumn(reverseRow(pushRow(reverseRow(row))));
    }
}

inline board_t game_t::initBoard(){
    board_t board = 0;
    spawnTile(board);
    spawnTile(board);
    return board;
}

void game_t::print(board_t board){
    for(index_t i = 0; i < N; ++i){
        for(index_t j = 0; j < N; ++j){
            printf("%2d ", getCell(board, i, j));
        }
        printf("\n");
    }
    printf("\n");
}

inline void game_t::setCell(board_t &board, index_t index, value_t value){
    board &= ~(0xFULL << exponentBits * index);
    board |= ((board_t)value << exponentBits * index);
}

inline void game_t::setCell(board_t &board, index_t i, index_t j, value_t value){
    setCell(board, i*N+j, value);
}

inline value_t game_t::getCell(board_t board, index_t index){
    return (board >> exponentBits * index) & 0xFULL;
}

inline value_t game_t::getCell(board_t board, index_t i, index_t j){
    return getCell(board, i*N+j);
}

inline void game_t::spawnTile(board_t &board){
    std::vector<index_t> empty;
    empty.reserve(16);
    for(index_t index = 0; index < N*N; ++index){
        if(!getCell(board, index))
            empty.push_back(index);
    }

    if(!empty.size())
        return;

    value_t spawn = distribution(generator) > 0.9 ? 2 : 1;
    if(empty.size() != 1){
    	index_t pick = (index_t)(distribution(generator) * empty.size());
    	board |= ((board_t)spawn << exponentBits * empty[pick]);
	} else {
		board |= ((board_t)spawn << exponentBits * empty[0]);
	}

}

inline score_t game_t::getScore(board_t board){
    score_t score = 0;
    for(index_t index = 0; index < N*N; ++index){
        score_t tile = getCell(board, index);
        if(tile < 2)
            continue;
        score += (tile-1)*pow(2, tile);
    }
    return score;
}

inline value_t game_t::getMaxTile(board_t board){
    value_t max = 0;
    for(index_t index = 0; index < N*N; ++index){
        value_t cell = getCell(board, index);
        if(cell > max)
            max = cell;
    }
    return max;
}

//transpose using 6 swaps
// 0 1 2 3          0 4 8 c
// 4 5 6 7          1 5 9 d
// 8 9 a b          2 6 a e
// c d e f          3 7 b f
inline board_t game_t::transpose(board_t board){
    board_t transposed = board;

    setCell(transposed, 1, getCell(board, 4));
    setCell(transposed, 4, getCell(board, 1));

    setCell(transposed, 2, getCell(board, 8));
    setCell(transposed, 8, getCell(board, 2));

    setCell(transposed, 3, getCell(board, 12));
    setCell(transposed, 12, getCell(board, 3));

    setCell(transposed, 6, getCell(board, 9));
    setCell(transposed, 9, getCell(board, 6));

    setCell(transposed, 7, getCell(board, 13));
    setCell(transposed, 13, getCell(board, 7));

    setCell(transposed, 11, getCell(board, 14));
    setCell(transposed, 14, getCell(board, 11));

    return transposed;
}

inline row_t game_t::reverseRow(row_t row){
    row_t reversed = 0;
    std::array<value_t, N> rowArray;

    rowArray[3] = row & 0x00F;
    rowArray[2] = (row >> exponentBits) & 0x00F;
    rowArray[1] = (row >> 2*exponentBits) & 0x00F;
    rowArray[0] = (row >> 3*exponentBits) & 0x00F;
    reversed = (rowArray[0]) + (rowArray[1] << exponentBits) + (rowArray[2] << 2*exponentBits) + (rowArray[3] << 3*exponentBits);
    return reversed;
}

static const board_t COLUMN_MASK = 0x000F000F000F000FULL;
board_t game_t::rowToColumn(row_t row){
    board_t tmpRow = row;
    return (tmpRow | (tmpRow << 12ULL) | (tmpRow << 24ULL) | (tmpRow << 36ULL)) & COLUMN_MASK;
}

row_t game_t::pushRow(row_t row){
    std::array<value_t, N> rowArray;

    rowArray[0] = row & 0x000F;
    rowArray[1] = (row >> exponentBits) & 0x000F;
    rowArray[2] = (row >> 2*exponentBits) & 0x000F;
    rowArray[3] = (row >> 3*exponentBits) & 0x000F;

    //printf("%d %d %d %d\n", rowArray[0], rowArray[1], rowArray[2], rowArray[3] );

    bool merged = false;
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
                    //pushScore += pow(2, rowArray[j]);
                    merged = true;
                    break;
                } else {
                    merged = false;
                    break;
                }
            }
        }
    }
    //printf("%d %d %d %d\n", rowArray[0], rowArray[1], rowArray[2], rowArray[3] );
    row_t result = (rowArray[0]) + (rowArray[1] << exponentBits) + (rowArray[2] << 2*exponentBits) + (rowArray[3] << 3*exponentBits);
    return result;
}

inline board_t game_t::moveRight(board_t board){
    board_t newBoard = board;
    newBoard ^= board_t(pushRowRight[(board >> 0) & 0xFFFFULL]) << 0;
    newBoard ^= board_t(pushRowRight[(board >> 16) & 0xFFFFULL]) << 16;
    newBoard ^= board_t(pushRowRight[(board >> 32) & 0xFFFFULL]) << 32;
    newBoard ^= board_t(pushRowRight[(board >> 48) & 0xFFFFULL]) << 48;
    return newBoard;
}

inline board_t game_t::moveLeft(board_t board){
    board_t newBoard = board;
    newBoard ^= board_t(pushRowLeft[(board >> 0) & 0xFFFFULL]) << 0;
    newBoard ^= board_t(pushRowLeft[(board >> 16) & 0xFFFFULL]) << 16;
    newBoard ^= board_t(pushRowLeft[(board >> 32) & 0xFFFFULL]) << 32;
    newBoard ^= board_t(pushRowLeft[(board >> 48) & 0xFFFFULL]) << 48;
    return newBoard;
}

inline board_t game_t::moveUp(board_t board){
    board_t newBoard = board;
    board_t transposed = transpose(board);
    newBoard ^= pushColUp[(transposed >>  0) & 0xFFFFULL] <<  0;
    newBoard ^= pushColUp[(transposed >> 16) & 0xFFFFULL] <<  4;
    newBoard ^= pushColUp[(transposed >> 32) & 0xFFFFULL] <<  8;
    newBoard ^= pushColUp[(transposed >> 48) & 0xFFFFULL] << 12;
    return newBoard;
}

inline board_t game_t::moveDown(board_t board){
    board_t newBoard = board;
    board_t transposed = transpose(board);
    newBoard ^= pushColDown[(transposed >>  0) & 0xFFFFULL] <<  0;
    newBoard ^= pushColDown[(transposed >> 16) & 0xFFFFULL] <<  4;
    newBoard ^= pushColDown[(transposed >> 32) & 0xFFFFULL] <<  8;
    newBoard ^= pushColDown[(transposed >> 48) & 0xFFFFULL] << 12;
    return newBoard;
}

inline board_t game_t::move(board_t board, move_t direction){
    switch(direction){
        case 0:
            return moveUp(board);
        case 1:
            return moveLeft(board);
        case 2:
            return moveDown(board);
        case 3:
            return moveRight(board);
    }
    //this should never happen
    return 0;
}

void game_t::getInput(board_t board, std::vector<double> &input){
    //input will be reset in genome::evaluate
    for(index_t index = 0; index < N*N; ++index){
        input[getCell(board, index)*16 + index] = 1.0;
    }
}

std::vector<move_t> game_t::sortOutput(std::vector<double> &output){
    std::vector<index_t> indices(output.size());
    std::iota(indices.begin(), indices.end(), static_cast<size_t>(0));
    std::sort(indices.begin(), indices.end(), [&](size_t a, size_t b){return output[a] > output[b];});
    return indices;
}

void game_t::play(genome* genom){
    std::vector<double> input(N*N*16, 0.0);
    std::vector<double> output(4);
    std::vector<move_t> sorted(4);
    bool legalmove;
    while(genom->precision > genom->poolPointer->targetPrecision){
        for(unsigned short run = 0; run < RUNS_PER_NETWORK; run++){
            board_t board = initBoard();
            while(true){
                getInput(board, input);
                genom->evaluate(input, output);
                std::iota(sorted.begin(), sorted.end(), static_cast<size_t>(0));
                std::sort(sorted.begin(), sorted.end(), [&](size_t a, size_t b){return output[a] > output[b];});
                legalmove = false;
                for(auto const& x : sorted){
                    board_t newBoard = move(board, x);
                    if(newBoard != board){
                        legalmove = true;
                        board = newBoard;
                        spawnTile(board);
                        break;
                    }
                }
                if(!legalmove)
                    break;
            }
            genom->scores.push_back(getScore(board));
        }
        genom->calculateFitness();
    }
    printf("Runs: %lu Score: %f Precision: %f\n", genom->scores.size(), genom->fitness, genom->precision);
}

void game_t::learn(){
    pool mainPool(N*N*16, 4, POPULATION);
    while(true){
        std::vector<genome*> allGenomes;
        allGenomes.reserve(POPULATION);
        for(auto const& spec : mainPool.speciesVector)
            for(auto const& genom : spec->genomes)
                allGenomes.push_back(genom);
        #pragma omp parallel for
        for(unsigned int i = 0; i < allGenomes.size(); ++i)
            play(allGenomes[i]);
        mainPool.newGeneration();
    }
}
