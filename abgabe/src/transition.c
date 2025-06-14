#include <stdlib.h> // for calloc, free, rand, EXIT_FAILURE
#include <stdbool.h> // for bool type
#include <stdio.h> // for fprintf, stderr

#include "include/transition.h" // for Transition, generate_random_transitions


void generate_random_transitions(Transition *trans, int count, int rows, int cols,
	                            int is_ladder, bool *used_start, bool *used_end)
{
	int board_size = rows * cols;

	if (!used_start || !used_end)
	{
		fprintf(stderr, "generate_random_transitions: calloc failed");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < count; ++i)
	{
		int start;
		int end;

		// generate until a valid, unused transition is found
		do
		{
			// pick a start square in [2..board_size-1] (no start at 1 or board_size)
			start = rand() % (board_size - 2) + 2;

			if (is_ladder)
			{
				// ladder: end in (start+1..board_size-1] (no end at board_size)
				int max_random_end = (board_size - 1) - start;
				int random_end;
				random_end = rand() % max_random_end + 1;
				end = start + random_end;
			}
			else
			{
				// snake: end in [2..start-1] (no end at 1)
				int max_random_end = (start - 2);
				int random_end;
				random_end = rand() % max_random_end + 1;
				end = start - random_end;
			}

			// repeat if invalid or duplicate
		} while (start == end
				|| end < 2
				|| end > board_size - 1
				|| used_start[start]
				|| used_end[end]);

		// assign and mark positions
		trans[i].start = start;
		trans[i].end = end;
	}
}
