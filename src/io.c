#include "io.h"
#include <stdlib.h>
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
int parse_args(int argc, char** argv, char *input_file, char *output_file, int parts, int margin, char *format) {
     input_file = NULL;
     output_file = NULL;
     parts = 2;
     margin = 10;
     format = "txt";
     // domyślne wartości
    int opt;
    
    while ((opt = getopt(argc, argv, "i:p:m:o:f:h")) != -1) {
        switch (opt) {
            case 'i':  // input
                input_file = optarg;
                break;
            case 'p':  // parts
                parts = atoi(optarg);
                if (parts <= 1) {
                    fprintf(stderr, "Błąd: Liczba części musi być większa od 1.\n");
                    return 3;
                }
                break;
            case 'm':  // margin
                margin = atoi(optarg);
                if (margin < 0 || margin > 50) {
                    fprintf(stderr, "Błąd: Margines procentowy musi być liczbą z zakresu 0-50.\n");
                    return 4;
                }
                break;
            case 'o':  // output
                output_file = optarg;
                break;
            case 'f':  // format
                if (strcmp(optarg, "txt") == 0 || strcmp(optarg, "bin") == 0) {
                    format = optarg;
                } else {
                    fprintf(stderr, "Błąd: Format musi być 'txt' lub 'bin'.\n");
                    return 5;
                }
            case 'h': 
                help();
                break;
            default:
                help();
                abort();
        }
    }
    return 0;
}
