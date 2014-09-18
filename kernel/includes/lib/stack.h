/*
 * Implementation of an unsorted, unordered mutable variable-length array.
 * Automagically allocates memory for new entries and releases memory that is
 * no longer required -- also doesn't explode randomly!
 */
#ifndef STACK_H
#define STACK_H

#include <types.h>
#include <lib/list.h>

typedef list_t stack;
typedef list_node_t stack_node;

// Allocation/deallocation functions
extern stack* stack_create();
extern void stack_destroy(stack*s);

extern void stack_push(stack *s, void* data);
extern void stack_pop(stack *s);
extern void *stack_top(stack *s);

#endif
