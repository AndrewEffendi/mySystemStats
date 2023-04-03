CC = gcc
CCFLAGS = -Wall -Werror
LDLIBS = -lm

all: mySystemStats

mySystemStats: mySystemStats.o tools.o cpu_functions.o memory_functions.o users_functions.o system_info_functions.o
	${CC} $^ -o $@ ${CCFLAGS} ${LDLIBS}

%.o: %.c
	${CC} -c $< 

.PHONY: clean
clean: 
	rm *.o mySystemStats

.PHONY: help
help: 
	@echo "all: run all"
	@echo "mySystemStats: link the object files and create a program called mySystemStats"
	@echo "%.o: compile c files"
	@echo "clean: remove all object files and executables"