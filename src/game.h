#include <cstdint>
#include <vector>
#include <array>
#include <random>
#include <algorithm>
#include "pool.h"
#include "random_t.h"

typedef uint64_t board_t;
typedef uint16_t row_t;
typedef uint8_t index_t;
typedef uint8_t value_t;
typedef uint8_t move_t;
typedef uint16_t score_t;

const uint8_t N = 4;
const uint8_t exponentBits = 4;

extern std::mt19937 generator;
extern std::uniform_real_distribution<> distribution;
extern std::uniform_int_distribution<> intDistribution;

extern std::vector<random_t> rng;

class game_t{
public:
    std::array<row_t, 65536> pushRowRight;
    std::array<row_t, 65536> pushRowLeft;
    std::array<board_t, 65536> pushColUp;
    std::array<board_t, 65536> pushColDown;

                    game_t();
    board_t         initBoard();
    void            print(board_t board);
    void            setCell(board_t &board, index_t index, value_t value);
    void            setCell(board_t &board, index_t i, index_t j, value_t value);
    value_t         getCell(board_t board, index_t index);
    value_t         getCell(board_t board, index_t i, index_t j);
    void            spawnTile(board_t &board);
    score_t         getScore(board_t board);
    value_t 		getMaxTile(board_t board);

    board_t         transpose(board_t board);
    row_t           reverseRow(row_t row);
    board_t         rowToColumn(row_t row);
    row_t           pushRow(row_t row);

    board_t         moveRight(board_t board);
    board_t         moveLeft(board_t board);
    board_t         moveUp(board_t board);
    board_t         moveDown(board_t board);
    board_t			move(board_t board, move_t direction);

    void					getInput(board_t board, std::vector<double> &input);
    std::vector<move_t>		sortOutput(std::vector<double> &output);

    void	        play(genome* genom);
    void 			learn();

};
