all:
	gcc  -Wall --pedantic src/*.c -o bin/graph_split
	./bin/graph_split