#include "graph.h"
// implementacja algorytmu FM
//
// inicjalizacja wierzchołków z informacjami potrzebnymi do korzystania z algorytmu FM
void initialize_vertices(Graph* graph, Vertex* vertices, int num_partitions);

// obliczanie zysków początkowych dla wierzchołka grafu
int calculate_initial_gain(Graph* graph, Vertex* vertices, int v);

// obliczanie zysku wynikającego z przesunięcia wierzchołka
int calculate_move_gain(Graph* graph, Vertex* vertices, int v, int target_partition);

// główna implementacja algorytmu
Graph* fm_partition(Graph* graf, int parts, int margin);
