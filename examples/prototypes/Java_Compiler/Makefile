JAVAC=javac
JAVA=java
SOURCES=$(wildcard *.java)
all:
	$(JAVAC) $(SOURCES)
run: all
	$(JAVA) Main input.java.txt
tokens: all
	$(JAVA) Main input.java.txt --tokens
tac: all
	$(JAVA) Main input.java.txt --tac
clean:
	rm -f *.class

