#define BUFFER_SIZE 1024
#include "../src/io.h"
#include "../src/graph.h"
int main() {
    int max_count = 0;


    char* indexes_buffer = malloc(BUFFER_SIZE );
    char* first_nodes_indexes_buffer = malloc(BUFFER_SIZE );
    char* groups_buffer = malloc(BUFFER_SIZE );
    char* first_group_indexes_buffer = malloc(BUFFER_SIZE );
    readFile("graf.csrrg",&max_count,indexes_buffer,first_nodes_indexes_buffer,groups_buffer,first_group_indexes_buffer);
   


    int* indexes = malloc(sizeof(int) * BUFFER_SIZE);
    int* first_nodes_indexes = malloc(sizeof(int) * BUFFER_SIZE);
    int* groups = malloc(sizeof(int) * BUFFER_SIZE);
    int* first_group_indexes = malloc(sizeof(int) * BUFFER_SIZE);

    int indexes_count = 0;
    int first_nodes_indexes_count = 0;
    int groups_count = 0;
    int first_group_indexes_count = 0;
    
    bufferToIntArray(indexes_buffer,indexes,BUFFER_SIZE,&indexes_count);
    bufferToIntArray(first_nodes_indexes_buffer,first_nodes_indexes,BUFFER_SIZE,&first_nodes_indexes_count);
    bufferToIntArray(groups_buffer,groups,BUFFER_SIZE,&groups_count);
    bufferToIntArray(first_group_indexes_buffer,first_group_indexes,BUFFER_SIZE,&first_group_indexes_count);
    

    for(int i =0; i < indexes_count; i++){
        printf("%d ", indexes[i]);
    }
    printf("\n");
    for(int i =0; i < first_nodes_indexes_count; i++){
        printf("%d ", first_nodes_indexes[i]);
    }
    printf("\n");
    for(int i =0; i < groups_count; i++){
        printf("%d ", groups[i]);
    }
    printf("\n");
    for(int i =0; i < first_group_indexes_count; i++){
        printf("%d ", first_group_indexes[i]);
    }
   
    printf("\n");
    Graph* new_graph = NULL;
    new_graph = create_graph(max_count, indexes, first_nodes_indexes, groups, first_group_indexes, first_nodes_indexes_count);
    display_graph(new_graph, max_count, first_nodes_indexes_count);
}
