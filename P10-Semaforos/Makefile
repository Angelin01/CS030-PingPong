# Code stuffs
INCL = queue.h datatypes.h pingpong.h buffer.h
EXE1 = pingpong-racecond
EXE2 = pingpong-semaphore
EXE3 = pingpong-prodcons
CFILES = queue.c pingpong.c buffer.c
OBJS = $(CFILES:.c=.o)
LIBS =

# Compiler stuffs
GCC = /usr/bin/gcc
FLAGS = -Wall
LIBPATH =
DEBUGFLAGS = -Wall -g -DDEBUG

default: $(EXE1) $(EXE2) $(EXE3)

# Compile library
%.o: %.c
	$(GCC) -c $(FLAGS) $*.c
	
# Link objects into binary
$(EXE1): $(OBJS) $(EXE1).o
	$(GCC) $(LIBPATH) $(OBJS) $(EXE1).o -o $@
	
$(EXE2): $(OBJS) $(EXE2).o
	$(GCC) $(LIBPATH) $(OBJS) $(EXE2).o -o $@
	
$(EXE3): $(OBJS) $(EXE3).o
	$(GCC) $(LIBPATH) $(OBJS) $(EXE3).o -o $@
		
# Object library depency
$(OBJS): $(INCL)

# Debug executable
debug:
	$(GCC) $(DEBUGFLAGS) $(CFILES) $(EXE1).c -o $(EXE1)_debug
	$(GCC) $(DEBUGFLAGS) $(CFILES) $(EXE2).c -o $(EXE2)_debug
	$(GCC) $(DEBUGFLAGS) $(CFILES) $(EXE3).c -o $(EXE3)_debug

# Clean
clean:
	rm -f $(EXE1) $(EXE2) $(EXE3) 
	rm -f $(EXE1)_debug $(EXE2)_debug $(EXE3)_debug
	rm -f $(OBJS) $(EXE1).o $(EXE2).o $(EXE3).o
