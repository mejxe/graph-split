#include "io.h"

#define BUFFER_SIZE 1024

int main(int argc, char** argv){
    help();
    int max_count = 0;


    char* indexes_buffer = malloc(BUFFER_SIZE );
    char* first_nodes_indexes_buffer = malloc(BUFFER_SIZE );
    char* groups_buffer = malloc(BUFFER_SIZE );
    char* first_group_indexes_buffer = malloc(BUFFER_SIZE );
    read_file("graf.csrrg",&max_count,indexes_buffer,first_nodes_indexes_buffer,groups_buffer,first_group_indexes_buffer);
   


    int* indexes = malloc(sizeof(int) * BUFFER_SIZE);
    int* first_nodes_indexes = malloc(sizeof(int) * BUFFER_SIZE);
    int* groups = malloc(sizeof(int) * BUFFER_SIZE);
    int* first_group_indexes = malloc(sizeof(int) * BUFFER_SIZE);

    int indexes_count = 0;
    int first_nodes_indexes_count = 0;
    int groups_count = 0;
    int first_group_indexes_count = 0;
    
    buffer_to_int_array(indexes_buffer,indexes,BUFFER_SIZE,&indexes_count);
    buffer_to_int_array(first_nodes_indexes_buffer,first_nodes_indexes,BUFFER_SIZE,&first_nodes_indexes_count);
    buffer_to_int_array(groups_buffer,groups,BUFFER_SIZE,&groups_count);
    buffer_to_int_array(first_group_indexes_buffer,first_group_indexes,BUFFER_SIZE,&first_group_indexes_count);
    

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
}