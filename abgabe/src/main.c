#include <stdio.h> // for printf, fprintf
#include <stdlib.h> // for EXIT_SUCCESS, EXIT_FAILURE, srand
#include <unistd.h> // for getopt on POSIX
#include <getopt.h> // declares optind, optarg, etc.
#include <time.h> // for time

#include "include/config.h" // for load_config, destroy_config, GameConfig
#include "include/simulator.h" // for run_mcmc
#include "include/stats.h" // for print_mcmc_statistics, destroy_statistics


/**
 * @brief prints usage instructions for the program
 * @param prog name of the executable (argv[0])
 * @returns void
 * @note called when invalid command-line options are provided
 */
static void usage(const char *prog)
{
    fprintf(stderr, "usage: %s [-c config.txt]\n"
            "-c <file>    optional path to config file (default: config.txt)\n", prog);
}

int main(int argc, char **argv)
{
    const char *config_path = "config.txt";
    int option;

    srand((unsigned int)time(NULL));

    // parse command-line options
    while ((option = getopt(argc, argv, "c:")) != -1)
    {
        if (option == 'c')
        {
            config_path = optarg;
        }
        else
        {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    // error if extra arguments remain
    if (optind < argc)
    {
        fprintf(stderr, "error: too many arguments.\n");
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    GameConfig *config = load_config(config_path);
    if (!config)
    {
        fprintf(stderr, "failed to load configuration from '%s'\n", config_path);
        return EXIT_FAILURE;
    }

    // perform monte carlo simulations
    Statistics *stats = run_mcmc(config, config->num_runs);
    if (!stats)
    {
        fprintf(stderr, "error: simulation failed\n");
        destroy_config(config);
        return EXIT_FAILURE;
    }

    // output statistics
    print_mcmc_statistics(stats, config);

    // cleanup resources
    destroy_statistics(stats);
    destroy_config(config);
    return EXIT_SUCCESS;
}
