
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include <time.h>

// Vertex structure
typedef struct Vertex {
    int id;
    char* name;
    int gain;
    struct Vertex* next;
    struct Vertex** neighbors;
    int neighbor_count;
    int max_neighbors;
} Vertex;

// Graph structure
typedef struct Graph {
    Vertex** vertices;
    int vertex_count;
    int max_vertices;
    int max_degree;
    Vertex*** left_buckets;
    Vertex*** right_buckets;
    bool* partition;
    int max_left_gain_idx;
    int max_right_gain_idx;
    bool current_side;
} Graph;

