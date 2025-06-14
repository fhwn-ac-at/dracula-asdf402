#ifndef CONFIG_H
#define CONFIG_H


#include "board.h" // for Board type


/**
 * @brief holds all game field data and configuration for simulations
 */
typedef struct GAMECONFIG_STRUCT
{
    int num_snakes; // number of snakes
    int num_ladders; // number of ladders
    int num_runs; // number of simulation runs
    int max_steps; // abort after max_steps die rolls

    Board *board; // pointer to the board object
    
    Transition *snakes; // array of snake transitions
    Transition *ladders; // array of ladder transitions
} GameConfig;

/**
 * @brief loads configuration from a file (e.g., text or json)
 * @param path path to the config file
 * @returns pointer to the loaded GameConfig, or null on failure
 * @note uses defaults if file is missing or invalid
 */
GameConfig *load_config(const char *path);

/**
 * @brief frees all resources of a GameConfig
 * @param config pointer to the GameConfig to destroy
 * @returns void
 */
void destroy_config(GameConfig *config);


#endif
