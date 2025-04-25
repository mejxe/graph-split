#ifndef GRAPH_H
#define GRAPH_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include "ds.h"

// Forward declarations

// Vertex functions
Vertex* create_vertex(const char* name);
void free_vertex(Vertex* v);
void add_neighbor(Vertex* v, Vertex* neighbor);
bool is_neighbor(Vertex* v, Vertex* u);

// Graph functions
Graph* create_graph();
void free_graph(Graph* g);
Vertex* find_vertex(Graph* g, const char* name);
Vertex* add_vertex_to_graph(Graph* g, const char* name);
void add_edge(Graph* g, const char* name1, const char* name2);
void calculate_max_degree(Graph* g);

// Bucket functions
void initialize_buckets(Graph* g);
void add_to_bucket(Vertex*** buckets, Vertex* v, int gain, int max_degree, int* max_gain_idx);
void initialize_bucket_sort(Graph* g, int vertex_idx, int gain, bool side);
void remove_from_bucket(Vertex*** buckets, Vertex* v, int gain, int max_degree);
void increment_gain(Graph* g, Vertex*** buckets, Vertex* v, bool side, int* max_gain_idx);
void decrement_gain(Graph* g, Vertex*** buckets, Vertex* v, bool side, int* max_gain_idx);

// Partition functions
void initialize_partition(Graph* g);
void randomize_partition(Graph* g);
int calculate_cost(Graph* g, bool* partition);
void initialize_gains(Graph* g);

// FM algorithm functions
void update_gains(Graph* g, bool* partition, bool* locked, int moved_idx);
int find_max_gain_vertex(Graph* g, bool* locked);
bool fm_pass(Graph* g, bool* partition, int* cost);
void fm_algorithm(Graph* g);

#endif /* GRAPH_H */

