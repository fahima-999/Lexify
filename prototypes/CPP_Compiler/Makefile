CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
SOURCES = main.cpp lexer.cpp parser.cpp ast.cpp semantic.cpp symbol_table.cpp interpreter.cpp codegen.cpp assembly.cpp
all: lexify_cpp
lexify_cpp: $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o lexify_cpp
clean:
	rm -f lexify_cpp lexify_cpp.exe
