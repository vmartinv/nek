#ifndef ORDERED_ARRAY_H
#define ORDERED_ARRAY_H

#include <types.h>
///Special sint8_t
typedef sint8_t (*lessthan_predicate_t)(type_t,type_t);
///An always sorted array
typedef struct
{
   type_t *array;
   uint32_t size;
   uint32_t max_size;
   lessthan_predicate_t less_than;
} ordered_array_t;

sint8_t standard_lessthan_predicate(type_t a, type_t b);

ordered_array_t create_ordered_array(uint32_t max_size, lessthan_predicate_t less_than);
ordered_array_t place_ordered_array(void *addr, uint32_t max_size, lessthan_predicate_t less_than);
///Destroys an ordered array
void destroy_ordered_array(ordered_array_t *array);
///Inserts into an array
void insert_ordered_array(type_t item, ordered_array_t *array);
///Find in ordered array
type_t lookup_ordered_array(uint32_t i, ordered_array_t *array);
///Remove from ordered array
void remove_ordered_array(uint32_t i, ordered_array_t *array);

#endif
