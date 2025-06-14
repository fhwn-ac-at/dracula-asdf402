#ifndef BOARD_H
#define BOARD_H


#include <stdbool.h> // for bool type

#include "transition.h" // for Transition type


/**
 * @brief represents the game board as a graph with jumps (snakes and ladders)
 */
typedef struct BOARD_STRUCT
{
    int rows; // number of rows
    int cols; // number of columns
    int num_squares; // total number of squares (rows * cols)
    int die_sides; // number of sides on the die
    bool exact_win; // true if must land exactly on the last square to win

    int *jumps; // array[1..num_squares]: jump destination for each start square
    int **possible_moves; // possible_moves[i][k]: landing square from i with roll k+1
    int *move_count; // move_count[i]: number of valid die outcomes (≤ die_sides)
} Board;

/**
 * @brief creates and initializes a new board
 * @param rows number of rows on the board
 * @param cols number of columns on the board
 * @param die_sides number of sides on the die
 * @param exact_win whether a player must land exactly on the final square to win
 * @param snakes array of snake transitions
 * @param num_snakes number of snakes in the snakes array
 * @param ladders array of ladder transitions
 * @param num_ladders number of ladders in the ladders array
 * @returns pointer to the newly allocated board, or null on allocation/validation failure
 * @note on failure, prints an error message to stderr and returns null
 */
Board *create_board(int rows, int cols, int die_sides, bool exact_win,
                    int num_snakes, int num_ladders,
                    Transition *ladders, Transition *snakes);

/**
 * @brief frees all resources associated with a board
 * @param board pointer to the board to destroy
 * @returns void
 */
void destroy_board(Board *board);


#endif
