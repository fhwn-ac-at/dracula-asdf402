# name of the final executable
exec = snakes_and_ladders.out

# grabs all .c files in src/
sources = $(wildcard src/*.c)

# directory for object files
objdir = obj

# transforms each source name into the corresponding .o (replace .c with .o)
# objects = $(sources:.c=.o) # for no extra directory
objects = $(patsubst src/%.c,$(objdir)/%.o,$(sources))

# compiler and flags
CC = clang
CFLAGS = -g -Iinclude -Wall -Wextra -Werror \
		-Wreturn-type -Wmissing-prototypes -Wconversion \
		-Wunused-parameter
LDFLAGS = -fopenmp # remove for non parallel path

# tells make, that all, install and clean are not real, only virtuel targets
.PHONY: all clean

# default target
all: $(exec)

# link
$(exec): $(objects)
	$(CC) $(objects) $(LDFLAGS) -o $(exec)

# rule to build objdir if needed
$(objdir):
	mkdir -p $(objdir)

# compile with clang
$(objdir)/%.o: src/%.c | $(objdir)
	$(CC) -c $(CFLAGS) $< -o $@

install: all
    # copy it into /usr/local/bin so it can be run system-wide as "snakes_and_ladders"
	cp ./$(exec) /usr/local/bin/snakes_and_ladders
 
# clean target: remove generated executables and object files
clean:
	-rm -f src/*.o $(exec)
	-rm -rf $(objdir)
