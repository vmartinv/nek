#include <sys/timer.h>
#include <stdlib.h>
#include <sys/logging.h>
#include <graphics/video.h>
#include <lib/list.h>
#include <types.h>

typedef struct timer_object{
	uint32_t delay, nxt;
	int id;
	int (*handler)(int);
} timer_object_t;

list_t *timers;
uint32_t elapsed=0;

inline static void insert_timer(timer_object_t * t)
{
	foreach(node, timers){
		timer_object_t *timer=(timer_object_t*)node->value;
		if(t->nxt <= timer->nxt){
			list_insert_before(timers, node, list_create_item(t));
			return;
		}
	}
	//PROBABLY a longggggggggg running task, appending to list
	list_append(timers, list_create_item(t));
}

void create_timer(int id,uint32_t delay,int (*handler)(int))
{
	timer_object_t * t = (timer_object_t*)malloc(sizeof(timer_object_t));
	t->id = id;
	t->delay = delay;
	t->nxt = elapsed+delay;
	t->handler = handler;
	insert_timer(t);
}

inline static void check_timers(){
	if(!timers || !timers->length)	return;
	timer_object_t *first_timer=(timer_object_t *)timers->head->value;
	if(first_timer->nxt <= elapsed){
		list_delete(timers, timers->head);
		if(!first_timer->handler(first_timer->id)){
			first_timer->nxt+=first_timer->delay;
			insert_timer(first_timer);
		}
		else
			free(first_timer);
		check_timers();
	}
}

void cycle_timers()
{
	elapsed++;
	check_timers();
}

void sleep(uint32_t ticks){
	uint32_t end=elapsed+ticks;
	int dummy;
	while(elapsed<end) dummy++;//why? maybe qemu bug TODO: check
}

uint32_t get_time(){
	return elapsed;
}
static int test_timer(int id)
{
	printk("timer","Timer %d Called!\n", id);
	return 0;
}

int init_timer()
{
	timers=list_create();
	return 0;
}
