#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "graph.h"

// informacje o programie
void help();

// funkcja do parsowania argumentów wywołania programu
int parse_args(int argc, char** argv, char **input_file, char **output_file, int *parts, int *margin, char **format);

// funkcja ładująca dane z plików w formacie csrrg do pamięci programu
void read_file(char* filename, int* max_count, char* indexes, char* first_nodes_indexes, char* groups, char* first_group_indexes);

// funkcja konwertujaca bufor na tablice intów 
void buffer_to_int_array (char* src, int* dest ,int size,int* copied_count);

// funkcja outputująca do pliku, w formacie TGF (Trival Graph Format)
void output(char* filename, Graph* split_graph);


void saveKWayPartitionTGF(const char *filename, Graph *graph, bool **partitions, int k, const char *input_filename);
void saveKWayPartitionBinary(const char *filename, Graph *graph, bool **partitions, int k, const char *input_filename);
