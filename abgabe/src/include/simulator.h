#ifndef SIMULATION_H
#define SIMULATION_H


#include <stdbool.h> // for bool type

#include "config.h" // for GameConfig, SimulationResult, Statistics types


/**
 * @brief stores the result of a single game simulation
 */
typedef struct SIMULATIONRESULT_STRUCT
{
    int num_rolls; // number of rolls until win or abort
    int *rolls; // optional sequence of dice rolls
    bool aborted; // true if max_steps reached without winning
} SimulationResult;

/**
 * @brief aggregated statistics over multiple simulations
 */
typedef struct STATISTICS_STRUCT
{
    double avg_rolls; // average number of rolls across all runs
    int min_rolls; // shortest number of rolls
    int max_rolls; // longest number of rolls
    int *best_rolls; // roll sequence for the shortest run
    long *traverse_count; // absolute count of each snake/ladder taken
    double *traverse_freq; // relative frequency (count/num_runs)
    int aborted_runs; // number of aborted simulations
    double aborted_freq; // fraction of runs that aborted
} Statistics;

/**
 * @brief runs a single simulation until win or max_steps
 * @param config pointer to game configuration
 * @returns pointer to simulation result (must be freed with destroy_simulation_result)
 */
SimulationResult *run_simulation(const GameConfig *config);

/**
 * @brief runs monte carlo simulations and computes statistics
 * @param config pointer to game configuration
 * @param num_runs number of independent simulations
 * @returns pointer to computed statistics (must be freed with destroy_statistics)
 */
Statistics *run_mcmc(const GameConfig *config, int num_runs);

/**
 * @brief computes statistics from an array of simulation results
 * @param results array of pointers to SimulationResult
 * @param num_runs length of results array
 * @param config pointer to game configuration for transitions
 * @returns pointer to computed statistics (must be freed with destroy_statistics)
 */
Statistics *compute_statistics(SimulationResult **results, int num_runs,
                              const GameConfig *config);

/**
 * @brief frees a SimulationResult and its associated memory
 * @param result pointer to the SimulationResult to destroy
 * @returns void
 */
void destroy_simulation_result(SimulationResult *result);

/**
 * @brief frees a Statistics object and its associated memory
 * @param stats pointer to the Statistics to destroy
 * @returns void
 */
void destroy_statistics(Statistics *stats);


#endif
