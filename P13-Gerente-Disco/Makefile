# Code stuffs
INCL = queue.h datatypes.h pingpong.h diskdriver.h harddisk.h
EXE1 = pingpong-disco
CFILES = queue.c pingpong.c harddisk.c
OBJS = $(CFILES:.c=.o)

# Compiler stuffs
GCC = /usr/bin/gcc
LIBS = -lm -lrt
FLAGS = -Wall
DEBUGFLAGS = -Wall -g -DDEBUG

default: $(EXE1)

# Compile library
%.o: %.c
	$(GCC) -c $(FLAGS) $*.c
	
# Link objects into binary
$(EXE1): $(OBJS) $(EXE1).o
	$(GCC) $(OBJS) $(EXE1).o $(LIBS) -o $@
		
# Object library depency
$(OBJS): $(INCL)

# Debug executable
debug:
	$(GCC) $(DEBUGFLAGS) $(CFILES) $(EXE1).c $(LIBS) -o $(EXE1)_debug

# Clean
clean:
	rm -f $(EXE1)
	rm -f $(EXE1)_debug
	rm -f $(OBJS) $(EXE1).o
