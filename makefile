CC = gcc
CCFLAGS = -Wall -Werror
LDLIBS = -lm

all: mySystemStats

mySystemStats: mySystemStats.o tools.o cpu_functions.o memory_functions.o users_functions.o system_info_functions.o
	${CC} $^ -o $@ ${CCFLAGS} ${LDLIBS}

%.o: %.c tools.h cpu_functions.h memory_functions.h users_functions.h system_info_functions.h
	${CC} -c $< 

.PHONY: clean
clean: 
	rm *.o mySystemStats

.PHONY: help
help: 
	@echo "all: run all"
	@echo "mySystemStats: link the object files and create a program called mySystemStats"
	@echo "%.o: compile all c files"
	@echo "clean: remove all object files and executables"