# Code stuffs
INCL = queue.h datatypes.h pingpong.h
EXE = pingpong-preempcao
CFILES = queue.c pingpong.c pingpong-preempcao.c
OBJS = $(CFILES:.c=.o)
LIBS =

# Compiler stuffs
GCC = /usr/bin/gcc
FLAGS = -Wall
LIBPATH =
DEBUGFLAGS = -Wall -g -DDEBUG

# Compile library
%.o: %.c
	$(GCC) -c $(FLAGS) $*.c

# Link objects into binary
$(EXE): $(OBJS)
	$(GCC) -o $(EXE) $(LIBPATH) $(OBJS)

# Object library depency
$(OBJS): $(INCL)

# Debug executable
debug:
	$(GCC) $(DEBUGFLAGS) $(CFILES) -o $(EXE)_debug

# Clean
clean:
	rm -f $(OBJS) $(EXE) $(EXE)_debug
