CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
SOURCES = main.cpp ast.cpp assembly.cpp codegen.cpp interpreter.cpp lex.yy.cpp parser.tab.cpp semantic.cpp symbol_table.cpp tac.cpp
TARGET = lexify
all: $(TARGET)
$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)
clean:
	rm -f $(TARGET) *.o
.PHONY: all clean
