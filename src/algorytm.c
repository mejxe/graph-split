#include "algorytm.h"
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

BucketStructure* initialize_buckets(int num_vertices, int max_degree) {
    BucketStructure* bs = (BucketStructure*)malloc(sizeof(BucketStructure));
    
    bs->max_gain = max_degree;
    bs->min_gain = -max_degree;
    int range = 2 * max_degree + 1;
    
    bs->bucket_sizes = (int*)calloc(range, sizeof(int));
    bs->buckets = (int**)malloc(range * sizeof(int*));
    for (int i = 0; i < range; i++) {
        bs->buckets[i] = (int*)malloc(num_vertices * sizeof(int));
    }
    
    bs->vertex_bucket = (int*)malloc(num_vertices * sizeof(int));
    
    return bs;
}

// Dodanie wierzchołka do odpowiedniego kubełka
void add_to_bucket(BucketStructure* bs, int vertex, int gain) {
    int bucket_index = gain - bs->min_gain;
    bs->buckets[bucket_index][bs->bucket_sizes[bucket_index]] = vertex;
    bs->vertex_bucket[vertex] = bucket_index;
    bs->bucket_sizes[bucket_index]++;
}

// Usunięcie wierzchołka z kubełka
void remove_from_bucket(BucketStructure* bs, int vertex, int gain) {
    int bucket_index = gain - bs->min_gain;
    
    // Znajdź pozycję wierzchołka w kubełku
    int pos = -1;
    for (int i = 0; i < bs->bucket_sizes[bucket_index]; i++) {
        if (bs->buckets[bucket_index][i] == vertex) {
            pos = i;
            break;
        }
    }
    
    if (pos != -1) {
        // Zastąp usuwanym wierzchołkiem ostatni element w kubełku
        bs->buckets[bucket_index][pos] = bs->buckets[bucket_index][bs->bucket_sizes[bucket_index] - 1];
        bs->bucket_sizes[bucket_index]--;
    }
}

// Znalezienie wierzchołka o najwyższym zysku
int get_max_gain_vertex(BucketStructure* bs) {
    // Przeszukaj kubełki od najwyższego zysku
    for (int gain = bs->max_gain; gain >= bs->min_gain; gain--) {
        int bucket_index = gain - bs->min_gain;
        if (bs->bucket_sizes[bucket_index] > 0) {
            // Zwróć pierwszy wierzchołek z kubełka o najwyższym zysku
            return bs->buckets[bucket_index][0];
        }
    }
    return -1; // Nie znaleziono wierzchołka
}

// Funkcja obliczająca początkowy zysk dla wierzchołka
int calculate_initial_gain(Graph* graph, Vertex* vertices, int v) {
    int gain = 0;
    Node* temp = graph->adj_lists[v];
    while (temp) {
        if (vertices[v].partition != vertices[temp->vertex].partition) {
            gain++;
        } else {
            gain--;
        }
        temp = temp->next;
    }
    return gain;
}

// Funkcja aktualizująca zysk wierzchołka
void update_vertex_gain(Graph* graph, Vertex* vertices, BucketStructure* bs, int v) {
    int old_gain = vertices[v].gain;
    int new_gain = calculate_initial_gain(graph, vertices, v);
    
    if (old_gain != new_gain) {
        remove_from_bucket(bs, v, old_gain);
        vertices[v].gain = new_gain;
        add_to_bucket(bs, v, new_gain);
    }
}

// Funkcja aktualizująca zyski sąsiadów po przeniesieniu wierzchołka
void update_neighbor_gains(Graph* graph, Vertex* vertices, BucketStructure* bs, int v) {
    Node* temp = graph->adj_lists[v];
    while (temp) {
        int neighbor = temp->vertex;
        if (!vertices[neighbor].locked) {
            update_vertex_gain(graph, vertices, bs, neighbor);
        }
        temp = temp->next;
    }
}

// Główna funkcja algorytmu FM
void fm_algorithm(Graph* graph, int num_partitions, int max_passes) {
    Vertex* vertices = (Vertex*)malloc(graph->num_vertices * sizeof(Vertex));
    int* best_partition = (int*)malloc(graph->num_vertices * sizeof(int));
    
    // Inicjalizacja wierzchołków
    for (int i = 0; i < graph->num_vertices; i++) {
        vertices[i].id = i;
        vertices[i].partition = i % num_partitions;
        vertices[i].locked = 0;
        vertices[i].gain = calculate_initial_gain(graph, vertices, i);
    }
    
    // Znajdź maksymalny stopień wierzchołka
    int max_degree = 0;
    for (int i = 0; i < graph->num_vertices; i++) {
        int degree = 0;
        Node* temp = graph->adj_lists[i];
        while (temp) {
            degree++;
            temp = temp->next;
        }
        if (degree > max_degree) max_degree = degree;
    }
    
    // Inicjalizuj strukturę kubełkową
    BucketStructure* bs = initialize_buckets(graph->num_vertices, max_degree);
    
    // Dodaj wierzchołki do kubełków
    for (int i = 0; i < graph->num_vertices; i++) {
        add_to_bucket(bs, i, vertices[i].gain);
    }
    
    int best_cut = INT_MAX;
    int current_cut = 0;
    
    // Oblicz początkową liczbę przeciętych krawędzi
    for (int i = 0; i < graph->num_vertices; i++) {
        Node* temp = graph->adj_lists[i];
        while (temp) {
            if (vertices[i].partition != vertices[temp->vertex].partition) {
                current_cut++;
            }
            temp = temp->next;
        }
    }
    current_cut /= 2; // Każda krawędź była liczona dwukrotnie
    
    // Główna pętla FM
    for (int pass = 0; pass < max_passes; pass++) {
        // Odblokuj wszystkie wierzchołki
        for (int i = 0; i < graph->num_vertices; i++) {
            vertices[i].locked = 1;
        }
        
        int moves = 0;
        int best_move = -1;
        int accumulated_gain = 0;
        
        // Wykonuj ruchy, dopóki są dostępne wierzchołki
        while (moves < graph->num_vertices) {
            int v = get_max_gain_vertex(bs);
            if (v == -1) break; // Brak dostępnych wierzchołków
            
            vertices[v].locked = 0;
            remove_from_bucket(bs, v, vertices[v].gain);
            
            // Zmień partycję wierzchołka
            int old_partition = vertices[v].partition;
            vertices[v].partition = (vertices[v].partition + 1) % num_partitions;
            
            // Aktualizuj liczbę przeciętych krawędzi
            current_cut -= vertices[v].gain;
            accumulated_gain += vertices[v].gain;
            
            // Aktualizuj zyski sąsiadów
            update_neighbor_gains(graph, vertices, bs, v);
            
            moves++;
            
            // Sprawdź, czy to najlepszy ruch w tym przebiegu
            if (current_cut < best_cut) {
                best_cut = current_cut;
                best_move = moves;
                
                // Zapisz aktualny podział jako najlepszy
                for (int i = 0; i < graph->num_vertices; i++) {
                    best_partition[i] = vertices[i].partition;
                }
            }
        }
        
        // Jeśli nie znaleziono lepszego podziału, zakończ
        if (best_move == -1) break;
        
        // Cofnij się do najlepszego znalezionego podziału
        for (int i = 0; i < graph->num_vertices; i++) {
            vertices[i].partition = best_partition[i];
        }
        
        // Jeśli nie ma poprawy, zakończ
        if (accumulated_gain <= 0) break;
    }
    
    // Wyświetl wynik
    printf("Najlepszy podział:\n");
    for (int i = 0; i < num_partitions; i++) {
        printf("Partycja %d: ", i);
        for (int j = 0; j < graph->num_vertices; j++) {
            if (best_partition[j] == i) {
                printf("%d ", j);
            }
        }
        printf("\n");
    }
    printf("Liczba przeciętych krawędzi: %d\n", best_cut);
    
    // Zwolnij pamięć
    free(vertices);
    free(best_partition);
    free(bs->bucket_sizes);
    for (int i = 0; i < 2 * max_degree + 1; i++) {
        free(bs->buckets[i]);
    }
    free(bs->buckets);
    free(bs->vertex_bucket);
    free(bs);
}
