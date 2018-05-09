CXX=g++
CXXFLAGS=-c
LFLAGS=-Wall -Wextra -pedantic -g
SRC=$(wildcard *.cpp)
OBJS=$(SRC:.cpp=.o)
TARGET=Logger


all: $(SRC) $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

.cpp.o:
	$(CXX) $(CXXFLAGS) $< -o $@ $(LFLAGS)

clean:
	rm *.o $(TARGET)

run:
	./$(TARGET)

val:
	valgrind ./$(TARGET)

backup:
	if [ -d "./backup" ]; then rm -r backup; fi
	mkdir backup
	cp $(SRC) backup
	cp *.h backup

class: 
	read -p "Enter Class Name:" name; \
	touch $$name.cpp; \
	printf "#include \""$$name.h"\"\n\n#include <iostream>\n\nusing namespace std;\n" > $$name.cpp; \
	touch $$name.h; \
	printf "#pragma once\n\nclass "$$name"{\npublic:\n\nprivate:\n\n};\n" > $$name.h

