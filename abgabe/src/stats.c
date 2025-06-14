#include <stdio.h> // for printf

#include "include/stats.h"  // for print_single_simulation, print_mcmc_statistics


void print_single_simulation(const SimulationResult *simulation_result)
{
	if (!simulation_result)
	{
		return;
	}

	// print total rolls and aborted status
	{
		const char *aborted_str;
		if (simulation_result->aborted)
		{
			aborted_str = " (aborted)";
		}
		else
		{
			aborted_str = "";
		}
		printf("single simulation finished in %d rolls %s\n", simulation_result->num_rolls, aborted_str);
	}

	// print detailed roll sequence if available
	if (simulation_result->num_rolls > 0)
	{
		printf("roll sequence:");
		for (int i = 0; i < simulation_result->num_rolls; ++i)
		{
			printf(" %d", simulation_result->rolls[i]);
		}
		printf("\n");
	}
}

void print_mcmc_statistics(const Statistics *stats, const GameConfig *config)
{
	if (!stats || !config)
	{
		return;
	}

	// used configs
	printf("used configs:\n");
	printf("  columns: %d\n", config->board->cols);
	printf("  rows: %d\n", config->board->rows);
	printf("  dice size: %d\n", config->board->die_sides);
	printf("  runs: %d\n", config->num_runs);
	printf("  ladders: %d\n", config->num_ladders);
	printf("  snakes: %d\n", config->num_snakes);
	printf("\n");


	// header with run count
	printf("MCMC simulation (%d runs) statistics:\n", config->num_runs);

	// summary statistics
	printf("  average rolls: %.2f\n", stats->avg_rolls);
	printf("  min rolls: %d\n", stats->min_rolls);
	printf("  max rolls: %d\n", stats->max_rolls);
	printf("  aborted runs: %d (%.2f%%)\n", stats->aborted_runs, stats->aborted_freq * 100.0);
	printf("\n");

	// best (shortest) roll sequence
	if (stats->min_rolls > 0)
	{
		printf("  best sequence (%d rolls):", stats->min_rolls);
		for (int i = 0; i < stats->min_rolls; ++i)
		{
			printf(" %d", stats->best_rolls[i]);
		}
		printf("\n\n");
	}

	// transition traversal counts and frequencies
	int num_transitions = config->num_snakes + config->num_ladders;

	for (int i = 0; i < num_transitions; ++i)
	{
		const char *type;
		int start;
		int end;

		if (i < config->num_snakes)
		{
			type = "snake";
			start = config->snakes[i].start;
			end   = config->snakes[i].end;
		}
		else
		{
			type = "ladder";
			start = config->ladders[i - config->num_snakes].start;
			end   = config->ladders[i - config->num_snakes].end;
		}

		printf("  %s %d->%d: count = %ld, freq = %.2f%%\n",
		       type, start, end,
		       stats->traverse_count[i],
		       stats->traverse_freq[i] * 100);
	}
}
