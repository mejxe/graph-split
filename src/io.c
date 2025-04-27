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
int parseArgs(int argc, char** argv, char **input_file, char **output_file, int *parts, int *margin, char **format) {
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

void readFile(char* filename, int* max_count, char* indexes, char* first_nodes_indexes, char* groups, char* first_group_indexes){
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


void bufferToIntArray (char* src, int* dest ,int size, int* copied_count){
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


// Funkcja pomocnicza do kopiowania pliku wejściowego do pliku wyjściowego
void copy_input_file_content(FILE *dest, const char *input_filename) {
    FILE *input = fopen(input_filename, "r");
    if (!input) {
        perror("Error opening input file for copying");
        return;
    }
    
    fprintf(dest, "# Input file: %s\n", input_filename);
    fprintf(dest, "# Original graph data below:\n");
    
    char buffer[4096]; // Bufor do kopiowania
    while (fgets(buffer, sizeof(buffer), input)) {
        fprintf(dest, "# %s", buffer);
    }
    
    fprintf(dest, "# End of input file\n\n");
    fclose(input);
}




void savePartitionTGF(const char *filename, Graph *graph, bool *partition) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error opening or creating TGF output file");
        return;
    }

    fprintf(file, "# Nodes (ID PartitionID)\n");
    for (int i = 0; i < graph->vertices; i++) {
        fprintf(file, "%d %d\n", i, (partition[i] != 0) ? 1 : 0); // Normalize to 0 or 1
    }

    fprintf(file, "#\n");
    for (int i = 0; i < graph->vertices; i++) {
        fprintf(file, "%d %d\n", i, (partition[i] != 0) ? 1 : 0); // Connect each node to its partition's representative (0 or 1)
    }

    for (int i = 0; i < graph->vertices; i++) {
        Edge *edge = graph->adjList[i];
        while (edge) {
            if (i < edge->dest) {
                fprintf(file, "%d %d\n", i, edge->dest);
            }
            edge = edge->next;
        }
    }

    fclose(file);
    printf("Partition saved to TGF file: %s\n", filename);
}



void savePartitionBinary(const char *filename, Graph *graph, bool *partition) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Error opening binary output file");
        return;
    }

    // Write the number of vertices
    int numVertices = graph->vertices;
    fwrite(&numVertices, sizeof(int), 1, file);

    // Write the partition information (0 or 1 for each vertex)
    fwrite(partition, sizeof(bool), numVertices, file);

    // Write the adjacency list
    for (int i = 0; i < numVertices; i++) {
        Edge *edge = graph->adjList[i];
        int edgeCount = 0;

        // First, count the number of edges for this vertex
        Edge *temp = graph->adjList[i];
        while (temp) {
            edgeCount++;
            temp = temp->next;
        }
        fwrite(&edgeCount, sizeof(int), 1, file); // Write the number of edges for vertex i

        while (edge) {
            fwrite(&edge->dest, sizeof(int), 1, file); // Write the destination vertex
            edge = edge->next;
        }
    }

    fclose(file);
    printf("Partition saved to binary file: %s\n", filename);


}






void saveKWayPartitionTGF(const char *filename, Graph *graph, bool **partitions, int k, const char *input_filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error opening or creating TGF output file");
        return;
    }

    // Add header with metadata
    fprintf(file, "# K-Way Graph Partition Output\n");
    fprintf(file, "# Number of partitions: %d\n", k);
    fprintf(file, "# Number of vertices: %d\n", graph->vertices);

    // Copy input file content
    if (input_filename) {
        fprintf(file, "\n");
        copy_input_file_content(file, input_filename);
    }

    // Write nodes section with partition IDs
    fprintf(file, "# Nodes (ID PartitionID)\n");
    for (int i = 0; i < graph->vertices; i++) {
        int partID = -1;
        for (int p = 0; p < k; p++) {
            if (partitions[p][i]) {
                partID = p;
                break;
            }
        }
        if (partID != -1) {
            fprintf(file, "%d %d\n", i, partID);
        } else {
            fprintf(file, "%d -1\n", i);  // Indicate unassigned node
        }
    }

    fprintf(file, "#\n");

    // Create connections from each node to its partition representative
    for (int i = 0; i < graph->vertices; i++) {
        int partID = -1;
        for (int p = 0; p < k; p++) {
            if (partitions[p][i]) {
                partID = p;
                break;
            }
        }
        if (partID != -1) {
            fprintf(file, "%d %d\n", i, partID); // Connect node to its partition representative
        }
    }

    // Write original edges
    for (int i = 0; i < graph->vertices; i++) {
        Edge *edge = graph->adjList[i];
        while (edge) {
            if (i < edge->dest) {
                fprintf(file, "%d %d\n", i, edge->dest);
            }
            edge = edge->next;
        }
    }

    fclose(file);
    printf("K-way partition saved to TGF file: %s\n", filename);
}




void saveKWayPartitionBinary(const char *filename, Graph *graph, bool **partitions, int k, const char *input_filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Error opening binary output file");
        return;
    }

    int numVertices = graph->vertices;
    
    // Write magic number to identify this is our format
    int magicNumber = 0x4B504152; // "KPAR" in hex
    fwrite(&magicNumber, sizeof(int), 1, file);
    
    // Write flags - bit 0: has input file
    int flags = input_filename ? 1 : 0;
    fwrite(&flags, sizeof(int), 1, file);
    
    // Write the number of vertices
    fwrite(&numVertices, sizeof(int), 1, file);
    
    // Write the number of partitions
    fwrite(&k, sizeof(int), 1, file);
    
    // If we have an input filename, write the length and the filename
    if (input_filename) {
        int input_filename_len = strlen(input_filename);
        fwrite(&input_filename_len, sizeof(int), 1, file);
        fwrite(input_filename, sizeof(char), input_filename_len, file);
        
        // Now copy the content of the input file
        FILE *input = fopen(input_filename, "r");
        if (input) {
            // Get file size
            fseek(input, 0, SEEK_END);
            long input_size = ftell(input);
            rewind(input);
            
            // Write input file size
            fwrite(&input_size, sizeof(long), 1, file);
            
            // Allocate buffer and read the entire file
            char *buffer = (char*)malloc(input_size);
            if (buffer) {
                size_t read_size = fread(buffer, 1, input_size, input);
                fwrite(buffer, 1, read_size, file);
                free(buffer);
            }
            fclose(input);
        }
    }
    
    // Create an array to store partition IDs (0 to k-1) for each vertex
    int *partitionIDs = (int*)malloc(numVertices * sizeof(int));
    if (!partitionIDs) {
        perror("Memory allocation failed");
        fclose(file);
        return;
    }
    
    // Determine partition ID for each vertex
    for (int v = 0; v < numVertices; v++) {
        partitionIDs[v] = -1;  // Default (error state)
        for (int p = 0; p < k; p++) {
            if (partitions[p][v]) {
                partitionIDs[v] = p;
                break;
            }
        }
    }
    
    // Write partition IDs for all vertices
    fwrite(partitionIDs, sizeof(int), numVertices, file);
    
    // Write the adjacency list
    for (int i = 0; i < numVertices; i++) {
        Edge *edge = graph->adjList[i];
        int edgeCount = 0;

        // Count the number of edges for this vertex
        Edge *temp = graph->adjList[i];
        while (temp) {
            edgeCount++;
            temp = temp->next;
        }
        
        // Write the number of edges for vertex i
        fwrite(&edgeCount, sizeof(int), 1, file);

        // Write each edge
        while (edge) {
            fwrite(&edge->dest, sizeof(int), 1, file);
            edge = edge->next;
        }
    }

    free(partitionIDs);
    fclose(file);
    printf("K-way partition saved to binary file: %s\n", filename);
}

