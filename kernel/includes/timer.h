#ifndef TIMER_H
#define TIMER_H
#include <stdint.h>
#include <stddef.h>
typedef struct timer_object{
	uint32_t ticks;
	uint32_t repeat;
	char * name;
	
	void (*handler)(struct timer_object *);

	struct timer_object * next; //Internal
} timer_object_t;

#endif