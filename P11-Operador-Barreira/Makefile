# Code stuffs
INCL = queue.h datatypes.h pingpong.h buffer.h
EXE1 = pingpong-barrier
CFILES = queue.c pingpong.c buffer.c
OBJS = $(CFILES:.c=.o)
LIBS =

# Compiler stuffs
GCC = /usr/bin/gcc
FLAGS = -Wall
LIBPATH =
DEBUGFLAGS = -Wall -g -DDEBUG

default: $(EXE1)

# Compile library
%.o: %.c
	$(GCC) -c $(FLAGS) $*.c
	
# Link objects into binary
$(EXE1): $(OBJS) $(EXE1).o
	$(GCC) $(LIBPATH) $(OBJS) $(EXE1).o -o $@
		
# Object library depency
$(OBJS): $(INCL)

# Debug executable
debug:
	$(GCC) $(DEBUGFLAGS) $(CFILES) $(EXE1).c -o $(EXE1)_debug

# Clean
clean:
	rm -f $(EXE1)
	rm -f $(EXE1)_debug
	rm -f $(OBJS) $(EXE1).o
