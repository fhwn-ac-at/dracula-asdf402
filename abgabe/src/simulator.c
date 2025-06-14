#include <stdlib.h> // for malloc, free
#include <stdio.h> // for fprintf, stderr
#include <time.h> // for time
#include <limits.h> // for INT_MAX
#include <stdint.h> // for uintptr_t
#include <string.h> // for memcpy


#if __has_include(<omp.h>)
#include <omp.h> // for omp_get_thread_num
#endif 

#include "include/simulator.h" // for SimulationResult, Statistics, run_simulation_internal


/**
 * @brief runs a simulation in a thread-local context
 * @param config pointer to the game configuration
 * @param seed pointer to thread-local random seed
 * @returns allocated SimulationResult, or NULL on failure
 */
static SimulationResult *run_simulation_internal(const GameConfig *config, unsigned int *seed)
{
	int max_steps = config->max_steps;
	int num_squares = config->board->num_squares;
	int sides = config->board->die_sides;

	// allocate result structure and roll buffer
	SimulationResult *simulation_result = malloc(sizeof(*simulation_result));
	if (!simulation_result)
	{
		return NULL;
	}

	simulation_result->rolls = malloc(sizeof(int) * (size_t)max_steps);
	if (!simulation_result->rolls)
	{
		free(simulation_result);
		return NULL;
	}

	simulation_result->num_rolls = 0;

	int position = 1;
	while (simulation_result->num_rolls < max_steps && position != num_squares)
	{
		int roll  = (int)(rand_r(seed) % sides) + 1;
		simulation_result->rolls[simulation_result->num_rolls++] = roll;
		int cand = position + roll;
		int next;

		if (cand > num_squares)
		{
			// handle overshoot based on exact_win flag
			if (config->board->exact_win)
			{
				next = position;
			}
			else
			{
				next = num_squares;
			}
		}
		else
		{
			next = config->board->jumps[cand];
		}

		position = next;
	}

	// mark as aborted if final square not reached
	simulation_result->aborted = (position != num_squares);
	return simulation_result;
}

SimulationResult *run_simulation(const GameConfig *config)
{
	unsigned int seed = (unsigned int)time(NULL) ^ (unsigned int)(uintptr_t)config; // ^ is a XOR
	return run_simulation_internal(config, &seed);
}

Statistics *compute_statistics(SimulationResult **simulation_result, int num_runs,
                              const GameConfig *config)
{
	// allocate stats structure
	Statistics *stats = malloc(sizeof(*stats));
	if (!stats)
	{
		return NULL;
	}

	// initialize average, minimum, and maximum rolls
	stats->min_rolls = INT_MAX;
	stats->max_rolls = 0;
	double sum = 0;

	for (int i = 0; i < num_runs; ++i)
	{
		int r = simulation_result[i]->num_rolls;
		sum += r;

		if (r < stats->min_rolls)
		{
			stats->min_rolls = r;
		}

		if (r > stats->max_rolls)
		{
			stats->max_rolls = r;
		}
	}

	stats->avg_rolls = sum / num_runs;

	// count aborted simulations
	stats->aborted_runs = 0;
	for (int i = 0; i < num_runs; ++i)
	{
		if (simulation_result[i]->aborted)
		{
			stats->aborted_runs++;
		}
	}

	stats->aborted_freq = stats->aborted_runs / (double)num_runs;

	// copy the best (shortest) roll sequence
	int best_idx = 0;
	for (int i = 0; i < num_runs; ++i)
	{
		if (simulation_result[i]->num_rolls == stats->min_rolls)
		{
			best_idx = i;
			break;
		}
	}

	stats->best_rolls = malloc(sizeof(int) * (size_t)stats->min_rolls);
	if (!stats->best_rolls)
	{
		free(stats);
		return NULL;
	}

	memcpy(stats->best_rolls, simulation_result[best_idx]->rolls,
	       sizeof(int) * (size_t)stats->min_rolls);

	// count absolute traversals of each transition
	int total_transitions  = config->num_snakes + config->num_ladders;
    
	stats->traverse_count = calloc((size_t)total_transitions , sizeof(long));
	if (!stats->traverse_count)
	{
		free(stats->best_rolls);
		free(stats);
		return NULL;
	}

	int num_squares = config->board->num_squares;
	for (int i = 0; i < num_runs; ++i)
	{
		int position = 1;

		for (int j = 0; j < simulation_result[i]->num_rolls; ++j)
		{
			int roll = simulation_result[i]->rolls[j];
			int cand = position + roll;
			int next;

			if (cand > num_squares)
			{
				if (config->board->exact_win)
				{
					next = position;
				}
				else
				{
					next = num_squares;
				}
			}
			else
			{
				next = config->board->jumps[cand];
			}

			// check if a jump occurred
			if (cand >= 1 && cand <= num_squares
				&& config->board->jumps[cand] != cand)
			{
				int transition_index  = -1;

				// find matching snake
				for (int s = 0; s < config->num_snakes; ++s)
				{
					if (config->snakes[s].start == cand
					 && config->snakes[s].end   == next)
					{
						transition_index  = s;
						break;
					}
				}

				// if not a snake, check ladders
				if (transition_index  < 0)
				{
					for (int l = 0; l < config->num_ladders; ++l)
					{
						if (config->ladders[l].start == cand
						 && config->ladders[l].end   == next)
						{
							transition_index  = config->num_snakes + l;
							break;
						}
					}
				}

				if (transition_index  >= 0)
				{
					stats->traverse_count[transition_index ]++;
				}
			}

			position = next;
		}
	}

	// calculate relative frequencies for each transition
	stats->traverse_freq = malloc(sizeof(double) * (size_t)total_transitions );
	if (!stats->traverse_freq)
	{
		free(stats->traverse_count);
		free(stats->best_rolls);
		free(stats);
		return NULL;
	}

	for (int i = 0; i < total_transitions ; ++i)
	{
		stats->traverse_freq[i] = (double)stats->traverse_count[i] / num_runs;
	}

	return stats;
}

Statistics *run_mcmc(const GameConfig *config, int num_runs)
{
	SimulationResult **simulation_result = malloc(sizeof(*simulation_result) * (size_t)num_runs);
	if (!simulation_result)
	{
		return NULL;
	}

	// if omp.h is installed -> use parallel programmin
	#if defined(_OPENMP)
	#pragma omp parallel
	{
		unsigned int seed = (unsigned int)time(NULL) ^ (unsigned int)omp_get_thread_num();
		
        #pragma omp for
		for (int i = 0; i < num_runs; ++i)
		{
			simulation_result[i] = run_simulation_internal(config, &seed);
		}
	}
	#else
	// if omp.h is not installed -> do one simulation at a time
    unsigned int seed = (unsigned int)time(NULL);
    for (int i = 0; i < num_runs; ++i)
    {
        simulation_result[i]
            = run_simulation_internal(config, &seed);
    }
#endif

	Statistics *stats = compute_statistics(simulation_result, num_runs, config);

	for (int i = 0; i < num_runs; ++i)
	{
		destroy_simulation_result(simulation_result[i]);
	}

	free(simulation_result);
	return stats;
}

void destroy_simulation_result(SimulationResult *result)
{
	if (!result)
	{
		return;
	}

	free(result->rolls);
	free(result);
}

void destroy_statistics(Statistics *stats)
{
	if (!stats)
	{
		return;
	}

	free(stats->best_rolls);
	free(stats->traverse_count);
	free(stats->traverse_freq);
	free(stats);
}
