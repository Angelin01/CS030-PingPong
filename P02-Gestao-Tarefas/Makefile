# Code stuffs
INCL = datatypes.h pingpong.h
EXE1 = pingpong-tasks1
EXE2 = pingpong-tasks2
EXE3 = pingpong-tasks3
MAINC = pingpong.c
TASKS1 = pingpong-tasks1.c
TASKS2 = pingpong-tasks2.c
TASKS3 = pingpong-tasks3.c
OBJSMAIN = $(MAINC:.c=.o)
OBJS1 = $(OBJSMAIN) $(TASKS1:.c=.0)
OBJS2 = $(OBJSMAIN) $(TASKS2:.c=.0)
OBJS3 = $(OBJSMAIN) $(TASKS3:.c=.0)
LIBS =

# Compiler stuffs
GCC = /usr/bin/gcc
FLAGS = -Wall
LIBPATH =
DEBUGFLAGS = -Wall -g -DDEBUG

# Compile library
%.o: %.c
	$(GCC) -c $(FLAGS) $*.c

# Link objects into binary for tasks1
tasks1:
$(EXE1): $(OBJS1)
	$(GCC) -o $(EXE1) $(LIBPATH) $(OBJS1)
	
# Link objects into binary for tasks2
tasks2:
$(EXE2): $(OBJS2)
	$(GCC) -o $(EXE2) $(LIBPATH) $(OBJS2)
	
# Link objects into binary for tasks3
tasks3:
$(EXE3): $(OBJS3)
	$(GCC) -o $(EXE3) $(LIBPATH) $(OBJS3)

# Debug executable 1
debug:
	$(GCC) $(DEBUGFLAGS) $(MAINC) $(TASKS1) -o $(EXE1)_debug
	
# Debug executable 2
debug:
	$(GCC) $(DEBUGFLAGS) $(MAINC) $(TASKS2) -o $(EXE2)_debug
	
# Debug executable 3
debug:
	$(GCC) $(DEBUGFLAGS) $(MAINC) $(TASKS3) -o $(EXE3)_debug

# Clean
clean:
	rm -f *.o $(EXE1) $(EXE2) $(EXE3) $(EXE1)_debug $(EXE2)_debug $(EXE3)_debug
