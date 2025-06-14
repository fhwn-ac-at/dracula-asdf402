#ifndef TRANSITION_H
#define TRANSITION_H


#include <stdbool.h>


/**
 * @brief defines a board transition (snake or ladder) from one square to another
 */
typedef struct TRANSITION_STRUCT
{
    int start; // index of start square (1..rows*cols)
    int end; // index of end square (1..rows*cols), end != start
} Transition;

/**
 * @brief generates random snakes or ladders for a board
 * @param trans pointer to array of Transition to fill
 * @param count number of transitions to generate
 * @param rows number of rows on the board
 * @param cols number of columns on the board
 * @param is_ladder 1 if generating ladders (end > start), 0 for snakes (end < start)
 * @returns void
 * @note seeds random number generator and ensures no duplicate start/end positions
 */
void generate_random_transitions(Transition *trans, int count, int rows, int cols,
                                 int is_ladder, bool *used_start, bool *used_end);


#endif
