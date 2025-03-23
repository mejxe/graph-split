all:
	gcc  -Wall --pedantic src/*.c -o bin/graph_split
	./bin/graph_split

test_cmd:
	gcc  -Wall --pedantic src/io.c src/io.h tests/test.c -o bin/tests/test_split
	./bin/tests/test_split -i graph.csrg -p 5 -m 11 -o wynik.txt -f txt

test_parse:
	gcc  -Wall -ggdb --pedantic src/io.c src/io.h src/graph.h src/graph.c tests/test_parse.c -o bin/tests/test_parse
	./bin/tests/test_parse

