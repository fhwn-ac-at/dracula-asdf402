# Hello, World!

- Usage: "program_name" -c "file"    optional path to config file (default: config.txt)

- make file available
standard name snakes_and_ladders.out

- "make clean" to remove all object and binary files

## standard configs (in config.c)
**upper bounds for validation:**
- rows 1000
- columns 1000
- dice size 100
- transitions 10000 (snakes and ladders cant be bigger then "rows * columns - 1")
- number of runs 10000000

**default configuration values:**
- rows 10
- columns 10
- number of snakes 3
- number of ladders 5
- dice size 6
- exact win 1 -> 1 = exact win, 0 = overshoot counts
- number of runs 1000


## dependencies
For parallel prozessing is the omp.h header required. You can also use the code without parallel prozessing but you need to delete the -fopenmp flag in the makefile

- Ubuntu/Debian: `sudo apt install libomp-dev`

## config.txt file format
(# and blank lines are ignored)
(if config are missing or not the right format or in range then they will be ignored and standard configs will be used)

\# Board size
rows: 10
columns: 10

\# Number of snakes / ladders
number_of_snakes: 3
number_of_ladders: 5

\# Dice configuration
dice_size: 6
exact_win: 1    # 1 = win exactly on the last square, 0 = overshoot counts

\# Number of simulations
number_of_runs: 1000
