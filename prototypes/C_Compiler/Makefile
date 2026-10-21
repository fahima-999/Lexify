CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
SOURCES = main.cpp ast.cpp assembly.cpp codegen.cpp interpreter.cpp lex.yy.cpp parser.tab.cpp semantic.cpp symbol_table.cpp tac.cpp
HEADERS = ast.h assembly.h codegen.h interpreter.h lexer.h parser.tab.h semantic.h symbol_table.h tac.h
TARGET = lexify

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

run: $(TARGET)
	./$(TARGET) input.c

clean:
	rm -f $(TARGET) *.o

.PHONY: all run clean
