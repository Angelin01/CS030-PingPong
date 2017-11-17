# Code stuffs
INCL = queue.h datatypes.h pingpong.h
EXE1 = pingpong-racecond
EXE2 = pingpong-semaphore
COWN = queue.c pingpong.c
CFILES = $(COWN) $(EXE1).c $(EXE2).c
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

default: racecond semaphore
	
# Link objects into binary
racecond:
	$(EXE1): $(OBJS)
		$(GCC) -o $(EXE1) $(LIBPATH) $(OBJS)

semaphore:
	$(EXE2): $(OBJS)
		$(GCC) -o $(EXE2) $(LIBPATH) $(OBJS)
		
# Object library depency
$(OBJS): $(INCL)

# Debug executable
debug:
	$(GCC) $(DEBUGFLAGS) $(COWN) $(EXE1).c -o $(EXE1)_debug
	$(GCC) $(DEBUGFLAGS) $(COWN) $(EXE2).c -o $(EXE2)_debug

# Clean
clean:
	rm -f $(OBJS) $(EXE1) $(EXE2) $(EXE1)_debug $(EXE2)_debug
