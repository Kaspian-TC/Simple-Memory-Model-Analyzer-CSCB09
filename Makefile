FILE = testingfiles/mallefique.c

SimpleMemModAnalyzer: assignment2.c
	gcc -g -std=c99 -Wall -o SimpleMemModAnalyzer assignment2.c
run: SimpleMemModAnalyzer
	./SimpleMemModAnalyzer $(FILE)
gdb:
	gdb ./SimpleMemModAnalyzer $(FILE)