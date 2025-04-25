#include "ds.h"
// Create a new vertex
// Modify create_vertex to initialize the id
Vertex* create_vertex(const char* name) {
    Vertex* v = (Vertex*)malloc(sizeof(Vertex));
    v->id = -1;       // Will be set when added to graph
    v->name = strdup(name);
    v->gain = 0;
    v->next = NULL;
    v->max_neighbors = 10;
    v->neighbors = (Vertex**)malloc(v->max_neighbors * sizeof(Vertex*));
    v->neighbor_count = 0;
    return v;
}

// Free vertex memory
void free_vertex(Vertex* v) {
    if (v) {
        free(v->name);
        free(v->neighbors);
        free(v);
    }
}

// Add a neighbor to a vertex
void add_neighbor(Vertex* v, Vertex* neighbor) {
    // Resize if needed
    if (v->neighbor_count >= v->max_neighbors) {
        v->max_neighbors *= 2;
        v->neighbors = (Vertex**)realloc(v->neighbors, v->max_neighbors * sizeof(Vertex*));
    }
    
    // Check if neighbor already exists
    for (int i = 0; i < v->neighbor_count; i++) {
        if (v->neighbors[i] == neighbor) {
            return;
        }
    }
    
    v->neighbors[v->neighbor_count++] = neighbor;
}

// Check if two vertices are neighbors
bool is_neighbor(Vertex* v, Vertex* u) {
    for (int i = 0; i < v->neighbor_count; i++) {
        if (v->neighbors[i] == u) {
            return true;
        }
    }
    return false;
}

// Initialize a new graph
Graph* create_graph() {
    Graph* g = (Graph*)malloc(sizeof(Graph));
    g->max_vertices = 100;
    g->vertices = (Vertex**)malloc(g->max_vertices * sizeof(Vertex*));
    g->vertex_count = 0;
    g->max_degree = 0;
    g->left_buckets = NULL;
    g->right_buckets = NULL;
    g->partition = NULL;
    g->max_left_gain_idx = 0;
    g->max_right_gain_idx = 0;
    g->current_side = false;
    return g;
}

// Free graph memory
void free_graph(Graph* g) {
    if (g) {
        // Free vertices
        for (int i = 0; i < g->vertex_count; i++) {
            free_vertex(g->vertices[i]);
        }
        free(g->vertices);
        
        // Free buckets
        if (g->left_buckets) {
            for (int i = 0; i < 2 * g->max_degree + 1; i++) {
                free(g->left_buckets[i]);
            }
            free(g->left_buckets);
        }
        
        if (g->right_buckets) {
            for (int i = 0; i < 2 * g->max_degree + 1; i++) {
                free(g->right_buckets[i]);
            }
            free(g->right_buckets);
        }
        
        free(g->partition);
        free(g);
    }
}

// Find a vertex by name
Vertex* find_vertex(Graph* g, const char* name) {
    for (int i = 0; i < g->vertex_count; i++) {
        if (strcmp(g->vertices[i]->name, name) == 0) {
            return g->vertices[i];
        }
    }
    return NULL;
}


// Modify add_vertex_to_graph to set the id
Vertex* add_vertex_to_graph(Graph* g, const char* name) {
    Vertex* v = find_vertex(g, name);
    if (v) {
        return v;
    }
    
    // Resize if needed
    if (g->vertex_count >= g->max_vertices) {
        g->max_vertices *= 2;
        g->vertices = (Vertex**)realloc(g->vertices, g->max_vertices * sizeof(Vertex*));
    }
    
    v = create_vertex(name);
    v->id = g->vertex_count;  // Set the id to the index
    g->vertices[g->vertex_count++] = v;
    return v;
}


// Add an edge between two vertices
void add_edge(Graph* g, const char* name1, const char* name2) {
    Vertex* v1 = add_vertex_to_graph(g, name1);
    Vertex* v2 = add_vertex_to_graph(g, name2);
    
    add_neighbor(v1, v2);
    add_neighbor(v2, v1);
}

// Calculate the maximum degree in the graph
void calculate_max_degree(Graph* g) {
    int max_deg = 0;
    for (int i = 0; i < g->vertex_count; i++) {
        if (g->vertices[i]->neighbor_count > max_deg) {
            max_deg = g->vertices[i]->neighbor_count;
        }
    }
    g->max_degree = max_deg;
}

void initialize_buckets(Graph* g) {
    calculate_max_degree(g);
    
    int bucket_count = 2 * g->max_degree + 1;
    
    // Initialize left buckets
    g->left_buckets = (Vertex***)malloc(bucket_count * sizeof(Vertex**));
    for (int i = 0; i < bucket_count; i++) {
        g->left_buckets[i] = (Vertex**)malloc(sizeof(Vertex*));
        g->left_buckets[i][0] = NULL;
    }
    
    // Initialize right buckets
    g->right_buckets = (Vertex***)malloc(bucket_count * sizeof(Vertex**));
    for (int i = 0; i < bucket_count; i++) {
        g->right_buckets[i] = (Vertex**)malloc(sizeof(Vertex*));
        g->right_buckets[i][0] = NULL;
    }
}

// Add a vertex to a bucket
void add_to_bucket(Vertex*** buckets, Vertex* v, int gain, int max_degree, int* max_gain_idx) {
    int idx = max_degree + gain;
    
    // Update max gain index
    if (gain != 0) {
        *max_gain_idx = (idx > *max_gain_idx) ? idx : *max_gain_idx;
    } else {
        *max_gain_idx = (max_degree > *max_gain_idx) ? max_degree : *max_gain_idx;
    }
    
    // Add to bucket
    Vertex* head = buckets[idx][0];
    v->next = head;
    buckets[idx][0] = v;
    v->gain = gain;
}

// Initialize bucket sort
void initialize_bucket_sort(Graph* g, int vertex_idx, int gain, bool side) {
    if (side) {
        add_to_bucket(g->right_buckets, g->vertices[vertex_idx], gain, g->max_degree, &g->max_right_gain_idx);
    } else {
        add_to_bucket(g->left_buckets, g->vertices[vertex_idx], gain, g->max_degree, &g->max_left_gain_idx);
    }
}

// Remove a vertex from a bucket
void remove_from_bucket(Vertex*** buckets, Vertex* v, int gain, int max_degree) {
    int idx = max_degree + gain;
    
    Vertex* curr = buckets[idx][0];
    Vertex* prev = NULL;
    
    while (curr != NULL) {
        if (curr == v) {
            if (prev == NULL) {
                buckets[idx][0] = curr->next;
            } else {
                prev->next = curr->next;
            }
            v->next = NULL;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

// Increment gain of a vertex
void increment_gain(Graph* g, Vertex*** buckets, Vertex* v, bool side, int* max_gain_idx) {
    // Remove from current bucket
    remove_from_bucket(buckets, v, v->gain, g->max_degree);
    
    // Add to new bucket with increased gain
    int new_gain = v->gain + 2;
    add_to_bucket(buckets, v, new_gain, g->max_degree, max_gain_idx);
}

// Decrement gain of a vertex
void decrement_gain(Graph* g, Vertex*** buckets, Vertex* v, bool side, int* max_gain_idx) {
    // Remove from current bucket
    remove_from_bucket(buckets, v, v->gain, g->max_degree);
    
    // Add to new bucket with decreased gain
    int new_gain = v->gain - 2;
    
    // Update max gain index if needed
    int old_idx = g->max_degree + v->gain;
    if (buckets[old_idx][0] == NULL && *max_gain_idx == old_idx) {
        (*max_gain_idx)--;
    }
    
    add_to_bucket(buckets, v, new_gain, g->max_degree, max_gain_idx);
}
// Now modify update_gains to use the id instead of linear search
void update_gains(Graph* g, bool* partition, bool* locked, int moved_idx) {
    bool current_side = g->current_side;
    
    // Mark vertex as locked
    locked[moved_idx] = true;
    
    // Flip the partition of the moved vertex
    partition[moved_idx] = !partition[moved_idx];
    
    // Flip the current side for balance
    g->current_side = !g->current_side;
    
    // Update gains of neighbors
    Vertex* moved_vertex = g->vertices[moved_idx];
    for (int i = 0; i < moved_vertex->neighbor_count; i++) {
        // Use the id directly instead of searching
        int neighbor_idx = moved_vertex->neighbors[i]->id;
        
        if (!locked[neighbor_idx]) {
            if (partition[neighbor_idx] == current_side) {
                // Neighbor is in the same partition as the moved vertex was
                if (current_side) {
                    increment_gain(g, g->right_buckets, g->vertices[neighbor_idx], current_side, &g->max_right_gain_idx);
                } else {
                    increment_gain(g, g->left_buckets, g->vertices[neighbor_idx], current_side, &g->max_left_gain_idx);
                }
            } else {
                // Neighbor is in the opposite partition
                if (!current_side) {
                    decrement_gain(g, g->right_buckets, g->vertices[neighbor_idx], !current_side, &g->max_right_gain_idx);
                } else {
                    decrement_gain(g, g->left_buckets, g->vertices[neighbor_idx], !current_side, &g->max_left_gain_idx);
                }
            }
        }
    }
}

// Initialize the partition vector
void initialize_partition(Graph* g) {
    g->partition = (bool*)malloc(g->vertex_count * sizeof(bool));
    
    // Split vertices evenly
    int mid = ceil(g->vertex_count / 2.0);
    for (int i = 0; i < mid; i++) {
        g->partition[i] = false;
    }
    for (int i = mid; i < g->vertex_count; i++) {
        g->partition[i] = true;
    }
}

// Randomize the partition
void randomize_partition(Graph* g) {
    srand(time(NULL));
    
    for (int i = 0; i < g->vertex_count; i++) {
        int j = rand() % g->vertex_count;
        bool temp = g->partition[i];
        g->partition[i] = g->partition[j];
        g->partition[j] = temp;
    }
}

// Calculate the cost (cut size) of a partition
int calculate_cost(Graph* g, bool* partition) {
    int cost = 0;
    
    for (int i = 0; i < g->vertex_count; i++) {
        if (partition[i] == false) {  // Only check vertices in left partition
            Vertex* v = g->vertices[i];
            for (int j = 0; j < v->neighbor_count; j++) {
                // Find the index of the neighbor
                int neighbor_idx = -1;
                for (int k = 0; k < g->vertex_count; k++) {
                    if (g->vertices[k] == v->neighbors[j]) {
                        neighbor_idx = k;
                        break;
                    }
                }
                
                if (neighbor_idx != -1 && partition[neighbor_idx] == true) {
                    cost++;
                }
            }
        }
    }
    
    return cost;
}

// Initialize gains for all vertices
void initialize_gains(Graph* g) {
    for (int i = 0; i < g->vertex_count; i++) {
        int gain = 0;
        Vertex* v = g->vertices[i];
        
        for (int j = 0; j < v->neighbor_count; j++) {
            // Find the index of the neighbor
            int neighbor_idx = -1;
            for (int k = 0; k < g->vertex_count; k++) {
                if (g->vertices[k] == v->neighbors[j]) {
                    neighbor_idx = k;
                    break;
                }
            }
            
            if (neighbor_idx != -1) {
                if (g->partition[i] == g->partition[neighbor_idx]) {
                    gain--;
                } else {
                    gain++;
                }
            }
        }
        
        initialize_bucket_sort(g, i, gain, g->partition[i]);
    }
}


// Find the vertex with maximum gain
int find_max_gain_vertex(Graph* g, bool* locked) {
    Vertex*** current_bucket;
    int* max_gain_idx;
    
    if (g->current_side == false) {
        current_bucket = g->left_buckets;
        max_gain_idx = &g->max_left_gain_idx;
    } else {
        current_bucket = g->right_buckets;
        max_gain_idx = &g->max_right_gain_idx;
    }
    
    // Find the first unlocked vertex with maximum gain
    for (int offset = 0; offset < g->max_degree; offset++) {
        int idx = *max_gain_idx - offset;
        Vertex* curr = current_bucket[idx][0];
        
        while (curr != NULL) {
            // Find the index of this vertex
            int vertex_idx = -1;
            for (int i = 0; i < g->vertex_count; i++) {
                if (g->vertices[i] == curr) {
                    vertex_idx = i;
                    break;
                }
            }
            
            if (vertex_idx != -1 && !locked[vertex_idx]) {
                return vertex_idx;
            }
            
            curr = curr->next;
        }
    }
    
    // If no vertex found, flip sides and try again
    g->current_side = !g->current_side;
    return -1;  // No vertex found
}

// Run a single pass of the FM algorithm
bool fm_pass(Graph* g, bool* partition, int* cost) {
    // Initialize locked vertices
    bool* locked = (bool*)calloc(g->vertex_count, sizeof(bool));
    
    // Best cost in this pass
    int best_cost = *cost;
    
    // Best partition in this pass
    bool* best_partition = (bool*)malloc(g->vertex_count * sizeof(bool));
    memcpy(best_partition, partition, g->vertex_count * sizeof(bool));
    
    // Initialize buckets
    initialize_buckets(g);
    
    // Initialize gains
    initialize_gains(g);
    
    // Set which side to begin with for balance
    int num_zeros = 0;
    for (int i = 0; i < g->vertex_count; i++) {
        if (partition[i]) {
            num_zeros++;
        }
    }
    
    if (num_zeros > 0 && g->vertex_count / num_zeros < 2) {
        g->current_side = true;
    } else {
        g->current_side = false;
    }
    
    // Begin pass
    for (int i = 0; i < g->vertex_count; i++) {
        int vertex_idx = find_max_gain_vertex(g, locked);
        
        if (vertex_idx == -1) {
            continue;  // No unlocked vertex found
        }
        
        update_gains(g, partition, locked, vertex_idx);
        
        int tmp_cost = calculate_cost(g, partition);
        if (tmp_cost < best_cost) {
            best_cost = tmp_cost;
            memcpy(best_partition, partition, g->vertex_count * sizeof(bool));
        }
    }
    
    // Check if there was improvement
    bool improved = (best_cost < *cost);
    
    if (improved) {
        *cost = best_cost;
        memcpy(partition, best_partition, g->vertex_count * sizeof(bool));
    }
    
    free(locked);
    free(best_partition);
    
    return improved;
}

// Run the FM algorithm until no improvement
void fm_algorithm(Graph* g) {
    // Initialize partition
    initialize_partition(g);
    
    // Randomize partition
    randomize_partition(g);
    
    // Calculate initial cost
    int cost = calculate_cost(g, g->partition);
    
    // Run passes until no improvement
    int pass = 0;
    while (pass < 10) {
        bool improved = fm_pass(g, g->partition, &cost);
        if (!improved) {
            break;
        }
        pass++;
    }
    
    printf("Final partition cost: %d\n", cost);
}


