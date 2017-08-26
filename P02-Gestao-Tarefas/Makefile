#code stuffs
INCL = $(wildcard *.h)
TASKS1 = pingpong-tasks1
TASKS2 = pingpong-tasks2
TASKS3 = pingpong-tasks3
LIBS = 

OBJM = pingpong.o
OBJ1 = pingpong-tasks1.o
OBJ2 = pingpong-tasks2.o
OBJ3 = pingpong-tasks3.o

#compiler stuffs
GCC = /usr/bin/gcc
FLAGS = -Wall
debug: DEBUG = -g -DDEBUG
debug: tasks1 tasks2 tasks3

%.o: %.c $(INCL)
	$(GCC) $(FLAGS) $(DEBUG) -c $< -o $@
	
tasks1: $(OBJM) $(OBJ1)
	$(GCC) $(OBJM) $(OBJ1) $(FLAGS) $(LIBS) -o $(TASKS1)

tasks2: $(OBJM) $(OBJ2)
	$(GCC) $(OBJM) $(OBJ2) $(FLAGS) $(LIBS) -o $(TASKS2)

tasks3: $(OBJM) $(OBJ3)
	$(GCC) $(OBJM) $(OBJ3) $(FLAGS) $(LIBS) -o $(TASKS3)
	
clean:
	rm -f *.o $(TASKS1) $(TASKS2) $(TASKS3)