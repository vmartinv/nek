#ifndef TIMER_H
#define TIMER_H
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif
 
void create_timer(int id,uint32_t delay,int (*handler)(int));
void sleep(uint32_t ticks);
uint32_t get_time();

//system only
void cycle_timers();
int init_timer();


#ifdef __cplusplus
}
#endif
#endif
