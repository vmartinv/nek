#ifndef TIMER_H
#define TIMER_H
#include <types.h>

void create_timer(int id,uint32_t delay,int (*handler)(int));
void sleep(uint32_t ticks);

//system only
void cycle_timers();
#endif
