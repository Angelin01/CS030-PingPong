# Code stuffs
INCL = queue.h datatypes.h pingpong.h
EXE1 = pingpong-mqueue
CFILES = queue.c pingpong.c
OBJS = $(CFILES:.c=.o)

# Compiler stuffs
GCC = /usr/bin/gcc
LIBS = -lm
FLAGS = -Wall
DEBUGFLAGS = -Wall -g -DDEBUG

default: $(EXE1)

# Compile library
%.o: %.c
	$(GCC) -c $(FLAGS) $*.c
	
# Link objects into binary
$(EXE1): $(OBJS) $(EXE1).o
	$(GCC) $(LIBS) $(OBJS) $(EXE1).o -o $@
		
# Object library depency
$(OBJS): $(INCL)

# Debug executable
debug:
	$(GCC) $(LIBS) $(DEBUGFLAGS) $(CFILES) $(EXE1).c -o $(EXE1)_debug

# Clean
clean:
	rm -f $(EXE1)
	rm -f $(EXE1)_debug
	rm -f $(OBJS) $(EXE1).o
