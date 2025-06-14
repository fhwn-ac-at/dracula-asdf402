#ifndef STATS_H
#define STATS_H


#include "simulator.h" // for SimulationResult, Statistics types
#include "config.h" // for GameConfig type


/**
 * @brief prints the result of a single simulation to stdout
 * @param res pointer to the SimulationResult to print
 * @returns void
 */
void print_single_simulation(const SimulationResult *res);

/**
 * @brief prints mcmc statistics (avg, min, max, best sequence,
 *        count and freq of each transition) to stdout
 * @param stats pointer to the computed Statistics
 * @param cfg   pointer to the GameConfig for transition details
 * @returns void
 */
void print_mcmc_statistics(const Statistics *stats, const GameConfig *cfg);


#endif
