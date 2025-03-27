#include "graph.h"
// Struktura kubełkowa dla FM (dla możliwie najszybszego dostępu do elementów)
typedef struct {
    int max_gain;       // Maksymalny możliwy zysk
    int min_gain;       // Minimalny możliwy zysk
    int* bucket_sizes;  
    int** buckets;      // Tablica kubełków
    int* vertex_bucket; // Indeks kubełka dla każdego wierzchołka
} BucketStructure;

// inicjalizacja struktury kubełkowej
BucketStructure* initialize_buckets(int num_vertices, int max_degree);

// dodawanie do kubełka
void add_to_bucket(BucketStructure* bs, int vertex, int gain);

// usuwanie z kubełka
void remove_from_bucket(BucketStructure* bs, int vertex, int gain);

// implementacja algorytmu FM
//
// inicjalizacja wierzchołków z informacjami potrzebnymi do korzystania z algorytmu FM
void initialize_vertices(Graph* graph, Vertex* vertices, int num_partitions);

// obliczanie zysków początkowych dla wierzchołka grafu
int calculate_initial_gain(Graph* graph, Vertex* vertices, int v);

// obliczanie zysku wynikającego z przesunięcia wierzchołka
int calculate_move_gain(Graph* graph, Vertex* vertices, int v, int target_partition);

// znalezienie wierzchołka o najwyższym zysku
int get_max_gain_vertex(BucketStructure* bs);

// główna implementacja algorytmu
Graph* fm_partition(Graph* graf, int parts, int margin);
