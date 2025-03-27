#ifndef GRAPH_H
#define GRAPH_H
// Struktura reprezentująca węzeł w liście sąsiedztwa
typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

// Struktura reprezentująca graf
typedef struct {
    int num_vertices;
    int num_edges;
    Node** adj_lists; // lista sąsiedztwa potrzebna ze względu na rzadkość grafu
} Graph;

// Struktura używana do reprezentowania wierzchołka w algorytmie FM
typedef struct {
    int id;
    int gain;
    int partition;
    int locked; // 0/1
} Vertex;


// funkcja inicjujaca graf
Graph* create_graph(int num_vertices);

// pomocnicza funkcja dodajaca połaczenia między węzłami
void add_edge(Graph* graph, int src, int dest);

// printowanie graphu na potrzeby debuggowania
void display_graph(Graph* graph, int size_x, int size_y);

// Funkcja zwalniająca pamięć zajmowaną przez graf
void free_graph(Graph* graph);
 
void print_cut_edges(Graph* graph, int* partition);

void print_partition(int* partition, int num_vertices, int num_partitions);

void print_graph(Graph* graph);
#endif
