#include "graph.h"
#include <stdio.h>
#include <stdlib.h>

// Funkcja tworząca nowy graf
Graph* create_graph(int num_vertices) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->num_vertices = num_vertices;
    graph->num_edges = 0;
    graph->adj_lists = (Node**)malloc(num_vertices * sizeof(Node*));
    
    for (int i = 0; i < num_vertices; i++) {
        graph->adj_lists[i] = NULL;
    }
    
    return graph;
}

// Funkcja dodająca krawędź do grafu
void add_edge(Graph* graph, int src, int dest) {
    // Dodaj krawędź od src do dest
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->vertex = dest;
    new_node->next = graph->adj_lists[src];
    graph->adj_lists[src] = new_node;
    
    // Dodaj krawędź od dest do src (graf nieskierowany)
    new_node = (Node*)malloc(sizeof(Node));
    new_node->vertex = src;
    new_node->next = graph->adj_lists[dest];
    graph->adj_lists[dest] = new_node;
    
    graph->num_edges++;
}
// Funkcja wyświetlająca graf w formie list sąsiedztwa
void print_graph(Graph* graph) {
    printf("\n=== STRUKTURA GRAFU ===\n");
    printf("Liczba wierzchołków: %d\n", graph->num_vertices);
    printf("Liczba krawędzi: %d\n\n", graph->num_edges);
    
    for (int i = 0; i < graph->num_vertices; i++) {
        printf("Wierzchołek %d: ", i);
        Node* temp = graph->adj_lists[i];
        while (temp) {
            printf("%d -> ", temp->vertex);
            temp = temp->next;
        }
        printf("NULL\n");
    }
    printf("\n");
}

// Funkcja wyświetlająca podział grafu na partycje
void print_partition(int* partition, int num_vertices, int num_partitions) {
    printf("\n=== PODZIAŁ GRAFU NA PARTYCJE ===\n");
    
    for (int i = 0; i < num_partitions; i++) {
        printf("Partycja %d: ", i);
        for (int j = 0; j < num_vertices; j++) {
            if (partition[j] == i) {
                printf("%d ", j);
            }
        }
        printf("\n");
    }
    printf("\n");
}

// Funkcja wyświetlająca przecięte krawędzie
void print_cut_edges(Graph* graph, int* partition) {
    printf("=== PRZECIĘTE KRAWĘDZIE ===\n");
    int cut_count = 0;
    
    for (int i = 0; i < graph->num_vertices; i++) {
        Node* temp = graph->adj_lists[i];
        while (temp) {
            if (temp->vertex > i && partition[i] != partition[temp->vertex]) {
                printf("Krawędź (%d, %d) - między partycjami %d i %d\n", 
                       i, temp->vertex, partition[i], partition[temp->vertex]);
                cut_count++;
            }
            temp = temp->next;
        }
    }
    
    printf("\nLiczba przeciętych krawędzi: %d\n\n", cut_count);
}

