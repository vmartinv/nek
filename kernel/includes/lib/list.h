/**
List - (Credit Klange)
**/
#ifndef LIST_H
#define LIST_H
#include <stdint.h>
#include <stddef.h>
typedef struct node {
	struct node * next;
	struct node * prev;
	void * value;
} __attribute__((packed)) list_node_t;

typedef struct {
	list_node_t * head;
	list_node_t * tail;
	size_t length;
} __attribute__((packed)) list_t;

void list_destroy(list_t * list);
void list_free(list_t * list);
void list_append(list_t * list, list_node_t * item);
list_node_t * list_insert(list_t * list, void * item);
list_t * list_create(void);
list_node_t * list_find(list_t * list, void * value);
int list_index_of(list_t * list, void * value);
void list_remove(list_t * list, size_t index);
void list_delete(list_t * list, list_node_t * node);
list_node_t * list_pop(list_t * list);
list_node_t * list_dequeue(list_t * list);
list_t * list_copy(list_t * original);
void list_merge(list_t * target, list_t * source);

void list_append_after(list_t * list, list_node_t * before, list_node_t * node);
list_node_t * list_insert_after(list_t * list, list_node_t * before, void * item);

#define foreach(i, list) for (list_node_t * i = list->head; i != NULL; i = i->next)
#endif
