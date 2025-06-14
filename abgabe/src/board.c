#include <stdlib.h> // for malloc, free
#include <stdio.h>  // for fprintf, stderr
#include <stdbool.h> // for bool

#include "include/board.h" // for Board type, create_board, destroy_board


/**
 * @brief applies and validates a single transition (snake or ladder)
 * @param board pointer to the board where jumps[] will be updated
 * @param transition the transition to apply {start, end}
 * @param used_start array[num_squares+1] tracking occupied start positions
 * @param used_end array[num_squares+1] tracking occupied end positions
 * @param num_squares total number of squares on the board
 * @returns 1 on success, 0 on validation failure
 */
static int apply_transition(Board *board, Transition transition, bool *used_start, bool *used_end,
					int num_squares)
{
// 
	int start = transition.start;
	int end = transition.end;

    // check bounds
	if (start < 1 || start > num_squares || end < 1 || end > num_squares)
	{
		return 0;
	}

	// apply valid transition
	board->jumps[start] = end;
	used_start[start] = true;
	used_end[end] = true;
	return 1;
}

Board *create_board(int rows, int cols, int die_sides, bool exact_win,
					int num_snakes, int num_ladders,
					Transition *ladders, Transition *snakes)
{
	int num_squares = rows * cols;

	Board *board = malloc(sizeof(*board));
	if (!board)
	{
		return NULL;
	}

	board->rows = rows;
	board->cols = cols;
	board->num_squares = num_squares;
	board->die_sides = die_sides;
	board->exact_win = exact_win;

	// initialize jumps array to identity mapping
	board->jumps = malloc(((size_t)num_squares + 1) * sizeof(int));
	if (!board->jumps)
	{
		fprintf(stderr, "create_board: failed to allocate jumps[%d]\n", num_squares + 1);
		goto err_board;
	}

	for (int i = 1; i <= num_squares; ++i)
	{
		board->jumps[i] = i;
	}

	// allocate helper arrays for transition validation
	bool *used_start = calloc((size_t)num_squares + 1, sizeof *used_start);
	bool *used_end   = calloc((size_t)num_squares + 1, sizeof *used_end);
	if (!used_start || !used_end)
	{
		fprintf(stderr, "create_board: failed to allocate used_start/used_end helpers\n");
		goto err_jumps;
	}

    // generate snakes and ladders (random, ohne Überschneidungen)
    generate_random_transitions(snakes, num_snakes, rows, cols, 0,
                                used_start, used_end);
    generate_random_transitions(ladders, num_ladders, rows, cols, 1,
                                used_start, used_end);

    // apply snakes and ladders
    for (int i = 0; i < num_snakes; ++i)
    {
        if (!apply_transition(board, snakes[i], used_start,
                              used_end, num_squares))
        {
            fprintf(stderr, "create_board: invalid snake #%d (%d -> %d)\n",
                    i, snakes[i].start, snakes[i].end);
            goto err_trans;
        }
    }
    for (int i = 0; i < num_ladders; ++i)
    {
        if (!apply_transition(board,
                              ladders[i],
                              used_start,
                              used_end,
                              num_squares))
        {
            fprintf(stderr,
                    "create_board: invalid ladder #%d (%d -> %d)\n",
                    i,
                    ladders[i].start,
                    ladders[i].end);
            goto err_trans;
        }
    }

	free(used_start);
	free(used_end);

	// allocate possible_moves and move_count
	board->possible_moves = malloc(((size_t)num_squares + 1) * sizeof(int *));
	if (!board->possible_moves)
	{
		fprintf(stderr, "create_board: failed to allocate possible_moves[%d]\n", num_squares + 1);
		goto err_jumps;
	}

	board->move_count = malloc(((size_t)num_squares + 1) * sizeof(int));
	if (!board->move_count)
	{
		fprintf(stderr, "create_board: failed to allocate move_count[%d]\n", num_squares + 1);
		goto err_moves;
	}

	// index 0 is unused
	board->possible_moves[0] = NULL;
	board->move_count[0] = 0;

	// compute moves for each square
	for (int i = 1; i <= num_squares; ++i)
	{
		board->possible_moves[i] = malloc((size_t)die_sides * sizeof(int));
		if (!board->possible_moves[i])
		{
			fprintf(stderr, "create_board: failed to allocate possible_moves[%d][%d]\n",
				    i, die_sides);
			for (int j = 1; j < i; ++j)
			{
				free(board->possible_moves[j]);
			}
			goto err_moves;
		}
		
		for (int k = 1; k <= die_sides; ++k)
		{
			int target = i + k;
			if (target > num_squares)
			{
				if (exact_win)
				{
					target = i;
				}
				else
				{
					target = num_squares;
				}
			}
			else
			{
				target = board->jumps[target];
			}

			board->possible_moves[i][k - 1] = target;
		}

		board->move_count[i] = die_sides;
	}

	return board;

// error handling cleanup
err_trans:
	free(used_start);
	free(used_end);
err_moves:
	free(board->move_count);
	free(board->possible_moves);
err_jumps:
	free(board->jumps);
err_board:
	free(board);
	return NULL;
}

void destroy_board(Board *board)
{
	if (!board)
	{
		return;
	}

	// free each row of possible moves
	for (int i = 1; i <= board->num_squares; ++i)
	{
		free(board->possible_moves[i]);
	}

	free(board->possible_moves);

	// free jumps array and move count
	free(board->jumps);
	free(board->move_count);

	// free board structure
	free(board);
}
