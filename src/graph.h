typedef struct Edge {
    // struktura węzła grafu
    int node; // od którego węzła wychodzi
    struct Edge* to[]; // lista węzłów z którymi się łączy
} GraphNode;

typedef struct Graph {
    // struktura grafu
    int max_width; 
    GraphNode **graph_array; // macierz przedstawiajaca graf
} Graph;
Graph* create_graph(int max_count, int* indexes, int* first_nodes_indexes, int* groups, int* first_group_indexes, int first_node_indexes_count);
Graph* partition(Graph* graf, int parts, int margin);
void display_graph(Graph* graph, int size_x, int size_y);
