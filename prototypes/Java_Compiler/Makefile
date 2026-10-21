JAVAC=javac
JAVA=java
SOURCES=$(wildcard *.java)
all:
	$(JAVAC) $(SOURCES)
run: all
	$(JAVA) Main input.java.txt
tokens: all
	$(JAVA) Main input.java.txt --tokens
ast: all
	$(JAVA) Main input.java.txt --ast
tac: all
	$(JAVA) Main input.java.txt --tac
asm: all
	$(JAVA) Main input.java.txt --asm
clean:
	rm -f *.class
.PHONY: all run tokens ast tac asm clean
