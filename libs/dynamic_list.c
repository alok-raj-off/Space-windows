#ifndef list_c
#define list_c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// creating space for variable

typedef struct list
{
    char* name;
    struct list* point_next;
} list ;

list* point_next = NULL;

void add_to_list(const char* newvariable)
{
    printf("\nthe new item is : %s \n", newvariable);
    list* new_item = (list*)malloc(sizeof(list));
    if (new_item == NULL){
        printf("Memory allocation at list faild for variables !");
    }
    // storing the variable into the list
    new_item->name = strdup(newvariable);
    // linking this vraible to the ld vriable if it exist
    new_item->point_next = point_next;
    // this will point to next space , see that it points that malloc sttement to stor things up
    point_next = new_item;
}

int hash(char* item)
{
    list* current = point_next;
    while(current != NULL)
    {
        if (strcmp(item, current->name) == 0)
        {
            printf("the item exists  !");
            return 1;
        }
        current = current->point_next;
    }
    printf("no it do not exists !");
    return 0;
}
void free_list()
{
    list* current = point_next;
    list* next_node;
    while (current != NULL) {
        next_node = current->point_next;     // Save the next pointer
        free(current->name);       // Free the string we duplicated
        free(current);                 // Free the node itself
        current = next_node;           // Move on
    }
    point_next = NULL;
}


#endif
