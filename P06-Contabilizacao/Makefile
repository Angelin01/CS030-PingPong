#code stuffs
INCL = $(wildcard *.h)
NORMAL = pingpong-contab
PRIO = pingpong-contab-prio
LIBS = 

OBJM = pingpong.o queue.o
OBJNORMAL = pingpong-contab.o
OBJPRIO = pingpong-contab-prio.o

#compiler stuffs
GCC = /usr/bin/gcc
FLAGS = -Wall

default: normal

debug: DEBUG = -g -DDEBUG
debug: normal prio

%.o: %.c $(INCL)
	$(GCC) $(FLAGS) $(DEBUG) -c $< -o $@
	
normal: $(OBJM) $(OBJNORMAL)
	$(GCC) $(OBJM) $(OBJNORMAL) $(FLAGS) $(LIBS) -o $(NORMAL)

prio: $(OBJM) $(OBJPRIO)
	$(GCC) $(OBJM) $(OBJPRIO) $(FLAGS) $(LIBS) -o $(PRIO)
	
clean:
	rm -f *.o $(NORMAL) $(PRIO)
