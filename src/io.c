#include "io.h"



void help() {
    printf("Narzędzie służące do dzielenia grafu na n części.\n");
    printf("---------------------------------------------------");
    printf("Manual");
    printf("–input – ścieżka do pliku wejściowego zawierającego graf."
         "–parts – liczba części, na które ma zostać podzielony graf (domyślnie 2)."
         "–margin – dopuszczalna różnica w liczbie wierzchołków pomiędzy częściami (domyślnie 10)"
         "–output – ścieżka do pliku wyjściowego."
         "–format <txt|bin> – format pliku wyjściowego (domyślnie txt).");
    printf("Przykładowe wywołanie programu:./graph_split −−input graf.csrg −−parts 3 −−margin 5 −−output wynik.txt −−format txt");
}
// Parsowanie argumentów wejściowych
int parse_args(int argc, char** argv, char **input_file, char **output_file, int *parts, int *margin, char **format) {
     *parts = 2;
     *margin = 10;
     *format = "txt";
     // domyślne wartości
    int opt;
    
    while ((opt = getopt(argc, argv, "i:p:m:o:f:h")) != -1) {
        switch (opt) {
            case 'i':  // input
                *input_file = optarg;
                break;
            case 'p':  // parts
                *parts = atoi(optarg);
                if (*parts <= 1) {
                    fprintf(stderr, "Błąd: Liczba części musi być większa od 1.\n");
                    return 3;
                }
                break;
            case 'm':  // margin
                *margin = atoi(optarg);
                if (*margin < 0 || *margin > 50) {
                    fprintf(stderr, "Błąd: Margines procentowy musi być liczbą z zakresu 0-50.\n");
                    return 4;
                }
                break;
            case 'o':  // output
                *output_file = optarg;
                break;
            case 'f':  // format
                if (strcmp(optarg, "txt") == 0 || strcmp(optarg, "bin") == 0) {
                    *format = optarg;
                } else {
                    fprintf(stderr, "Błąd: Format musi być 'txt' lub 'bin'.\n");
                    return 5;
                }
                break;
            case 'h': // help
                help();
                break;
            default:
                help();
                abort();
        }
    }
    return 0;
}

void read_file(char* filename, int* max_count, char* indexes, char* first_nodes_indexes, char* groups, char* first_group_indexes){
    FILE* file = fopen(filename,"r");
    if( file == NULL) {
        perror("Błąd: Nie udało się otworzyć podanego pliku ");
        abort();
    }
    //char line_buffer[1024*65];
    char* line_buffer = malloc(LINE_BUFFER_SIZE);

    if(!line_buffer){
        perror("Błąd alokacji przy czytaniu pliku");
        abort();
    }
    
    int line_index = 0;

    while (fgets(line_buffer,LINE_BUFFER_SIZE, file))
    {
        
        switch (line_index)
        {
            case 0:
            *max_count = atoi(line_buffer);
            break;
            case 1:
            strcpy(indexes,line_buffer);
            //memcpy(indexes,line_buffer,LINE_BUFFER_SIZE);
            
            break;
            case 2:
            strcpy(first_nodes_indexes,line_buffer);
            //memcpy(first_nodes_indexes,line_buffer,LINE_BUFFER_SIZE);
            
            break;
            case 3:
            strcpy(groups,line_buffer);
           // memcpy(groups,line_buffer,LINE_BUFFER_SIZE);
          
            break;
            case 4:
            strcpy(first_group_indexes,line_buffer);
           // memcpy(first_group_indexes,line_buffer,LINE_BUFFER_SIZE);
          
            break;
        }
        line_index++;
    }
    free(line_buffer);
    fclose(file);

}


void buffer_to_int_array (char* src, int* dest ,int size, int* copied_count){
    char* t = src;
    int index = 0;
    while(*t){
        
        if(isdigit(*t)){
            int n = strtol(t,&t,10);
            dest[index] = n;
            index++;
        }
        else{
            t++;
        }
        if(index >= size) break;
    }
    *copied_count = index;
}
