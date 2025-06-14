#include <stdio.h> // for fopen, fclose, fprintf
#include <stdlib.h> // for malloc, free
#include <string.h> // for strlen, strchr
#include <ctype.h> // for isspace
#include <errno.h> // for errno

#include "include/config.h" // for GameConfig, load_config, destroy_config
#include "include/transition.h" // for Transition type, generate_random_transitions


// upper bounds for validation
#define MAX_ROWS 1000
#define MAX_COLUMNS 1000
#define MAX_DICE_SIZE 100
#define MAX_TRANSITIONS 10000
#define MAX_RUNS 10000000

// default configuration values
#define DEFAULT_ROWS 10
#define DEFAULT_COLUMNS 10
#define DEFAULT_NUM_SNAKES 3
#define DEFAULT_NUM_LADDERS 5
#define DEFAULT_DICE_SIZE 6
#define DEFAULT_EXACT_WIN 1 // 1 = exact win, 0 = overshoot counts
#define DEFAULT_NUM_RUNS 1000


static char *trim(char *s)
{
	char *end;

	while (isspace((unsigned char)*s))
	{
		s++;
	}

	if (*s == '\0')
	{
		return s;
	}

	end = s + strlen(s) - 1;

	while (end > s && isspace((unsigned char)*end))
	{
		end--;
	}

	end[1] = '\0';

	return s;
}

static int parse_int(const char *s, int *out, int min, int max)
{
	char *endptr;
	long v;

	errno = 0;

	v = strtol(s, &endptr, 10);

	if (errno != 0)
	{
		return 0;
	}

	if (endptr == s)
	{
		return 0;
	}

	if (*endptr != '\0')
	{
		return 0;
	}

	if (v < min || v > max)
	{
		return 0;
	}

	*out = (int)v;

	return 1;
}

static int parse_config_file(const char *path, int *out_rows, int *out_cols,
                              int *out_dice_size, int *out_exact_win, GameConfig *config)
{
	FILE *fp;
	char line[256];
	int tmp;
	int rows;
	int cols;
	int num_snakes; // ns
	int num_ladders; // nl
	int dice_size; // ds
	int exact_win; // ew
	int num_runs; // nr

	rows = -1;
	cols = -1;
	num_snakes = -1;
	num_ladders = -1;
	dice_size = -1;
	exact_win = -1;
	num_runs = -1;

	fp = fopen(path, "r");
	if (!fp)
	{
		return 0;
	}

	while (fgets(line, sizeof(line), fp))
	{
		char *trimmed_line;
		char *colon_position;
		char *key;
		char *value;

		trimmed_line = trim(line);

		if (*trimmed_line == '#' || *trimmed_line == '\0' || *trimmed_line == '\n')
		{
			continue;
		}

		colon_position = strchr(trimmed_line, ':');

		if (!colon_position)
		{
			continue;
		}

		*colon_position = '\0';

		key   = trim(trimmed_line);
		value = trim(colon_position + 1);

		if (strcmp(key, "rows") == 0)
		{
			if (!parse_int(value, &tmp, 1, MAX_ROWS))
			{
				goto fail;
			}

			rows = tmp;
		}
		else if (strcmp(key, "columns") == 0)
		{
			if (!parse_int(value, &tmp, 1, MAX_COLUMNS))
			{
				goto fail;
			}

			cols = tmp;
		}
		else if (strcmp(key, "dice_size") == 0)
		{
			if (!parse_int(value, &tmp, 2, MAX_DICE_SIZE))
			{
				goto fail;
			}

			dice_size = tmp;
		}
		else if (strcmp(key, "exact_win") == 0)
		{
			if (!parse_int(value, &tmp, 0, 1))
			{
				goto fail;
			}

			exact_win = tmp;
		}
		else if (strcmp(key, "number_of_snakes") == 0)
		{
			if (!parse_int(value, &tmp, 0, MAX_TRANSITIONS))
			{
				goto fail;
			}

			num_snakes = tmp;
		}
		else if (strcmp(key, "number_of_ladders") == 0)
		{
			if (!parse_int(value, &tmp, 0, MAX_TRANSITIONS))
			{
				goto fail;
			}

			num_ladders = tmp;
		}
		else if (strcmp(key, "number_of_runs") == 0)
		{
			if (!parse_int(value, &tmp, 1, MAX_RUNS))
			{
				goto fail;
			}

			num_runs = tmp;
		}
		else
		{
			fprintf(stderr, "Warning: unknown key '%s'\n", key);
		}
	}

	fclose(fp);

	if (rows < 0 ||
		cols < 0 ||
		dice_size < 0 ||
		exact_win < 0 ||
		num_snakes < 0 ||
		num_ladders < 0 ||
		num_runs < 0)
	{
		fprintf(stderr, "parse_config_file: missing entries, using defaults\n");

		return 0;
	}

	if (num_snakes > rows * cols - 1 || num_ladders > rows * cols - 1)
	{
		fprintf(stderr, "parse_config_file: too many snakes/ladders for %dx%d board\n",
			    rows, cols);

		return 0;
	}

	*out_rows = rows;
	*out_cols = cols;
	*out_dice_size = dice_size;
	*out_exact_win = exact_win;
	config->num_snakes = num_snakes;
	config->num_ladders = num_ladders;
	config->num_runs = num_runs;

	return 1;

fail:
	fclose(fp);
	fprintf(stderr, "parse_config_file: invalid format, using defaults\n");

	return 0;
}

static void init_config_defaults(GameConfig *config)
{
	config->num_snakes  = DEFAULT_NUM_SNAKES;
	config->num_ladders = DEFAULT_NUM_LADDERS;
	config->num_runs    = DEFAULT_NUM_RUNS;
}

GameConfig *load_config(const char *path)
{
	GameConfig *config;
	int rows;
	int cols;
	int dice_size;
	int exact_win;

	config = malloc(sizeof(*config));
	if (!config)
	{
		return NULL;
	}

	init_config_defaults(config);
	rows = DEFAULT_ROWS;
	cols = DEFAULT_COLUMNS;
	dice_size = DEFAULT_DICE_SIZE;
	exact_win = DEFAULT_EXACT_WIN;

	if (!parse_config_file(path, &rows, &cols, &dice_size, &exact_win, config))
	{
		fprintf(stderr, "load_config: could not open/parse '%s', using defaults\n", path);
	}

	if (config->num_snakes > 0)
	{
		config->snakes = malloc(sizeof(Transition) * (size_t)config->num_snakes);
		if (!config->snakes)
		{
			fprintf(stderr, "malloc snakes failed");
			free(config);

			return NULL;
		}
	}
	else
	{
		config->snakes = NULL;
	}

	if (config->num_ladders > 0)
	{
		config->ladders = malloc(sizeof(Transition) * (size_t)config->num_ladders);
		if (!config->ladders)
		{
			fprintf(stderr, "malloc ladders failed");
			free((void *)config->snakes);
			free(config);

			return NULL;
		}
	}
	else
	{
		config->ladders = NULL;
	}

	config->max_steps = rows * cols * dice_size * 2;

	config->board = create_board(rows, cols, dice_size, (bool)exact_win,
								config->num_snakes, config->num_ladders,
								config->ladders, config->snakes);

	if (!config->board)
	{
		destroy_config(config);

		return NULL;
	}

	return config;
}

void destroy_config(GameConfig *config)
{
	if (!config)
	{
		return;
	}

	destroy_board(config->board);

	free((void *)config->snakes);
	free((void *)config->ladders);
	free(config);
}
