# Code stuffs
INCL = queue.h datatypes.h pingpong.h
EXE1 = pingpong-racecond
EXE2 = pingpong-semaphore
CFILES = queue.c pingpong.c
OBJS = $(CFILES:.c=.o)
LIBS =

# Compiler stuffs
GCC = /usr/bin/gcc
FLAGS = -Wall
LIBPATH =
DEBUGFLAGS = -Wall -g -DDEBUG

default: $(EXE1) $(EXE2)

# Compile library
%.o: %.c
	$(GCC) -c $(FLAGS) $*.c
	
# Link objects into binary
$(EXE1): $(OBJS) $(EXE1).o
	$(GCC) $(LIBPATH) $(OBJS) $(EXE1).o -o $@
	
$(EXE2): $(OBJS) $(EXE2).o
	$(GCC) $(LIBPATH) $(OBJS) $(EXE2).o -o $@
		
# Object library depency
$(OBJS): $(INCL)

# Debug executable
debug:
	$(GCC) $(DEBUGFLAGS) $(CFILES) $(EXE1).c -o $(EXE1)_debug
	$(GCC) $(DEBUGFLAGS) $(CFILES) $(EXE2).c -o $(EXE2)_debug

# Clean
clean:
	rm -f $(OBJS) $(EXE1) $(EXE2) $(EXE1)_debug $(EXE2)_debug
