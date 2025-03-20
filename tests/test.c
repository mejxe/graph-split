#include "../src/io.h"
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char **argv) {
    int parts;
    int margin; 
    char* input_file = malloc(sizeof(char) * 30);
    char* output_file = malloc(sizeof(char) * 30);
    char* format = malloc(sizeof(char) * 30);
    parse_args(argc, argv,&input_file,&output_file, &parts, &margin, &format);
    printf("%s, %s, %s, %d, %d", input_file, output_file, format, parts, margin);
}
