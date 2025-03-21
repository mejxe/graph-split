#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

void help();
int parse_args(int argc, char** argv, char **input_file, char **output_file, int *parts, int *margin, char **format);

void read_file(char* filename, int* max_count, char* indexes, char* first_nodes_indexes, char* groups, char* first_group_indexes);
void buffer_to_int_array (char* src, int* dest ,int size,int* copied_count);