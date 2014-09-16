#include <timer.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
uint32_t allocated_timers = 0;
uint32_t active_timers = 0;

timer_object_t * timer_root = NULL;

timer_object_t * timer_default = NULL;

timer_object_t * create_timer(char * name,uint32_t ticks, uint32_t repeat,void (*handler)(struct timer_object *))
{
	printk("debug","Creating [%s], running in %d ticks, every %d ticks\n",name,ticks,repeat);
	timer_object_t * t = (timer_object_t*)malloc(sizeof(timer_object_t));
	t->name = name;
	t->ticks = ticks;
	t->repeat = repeat;
	t->handler = handler;
	allocated_timers++;
	return t;
}

int insert_timer(timer_object_t * t)
{
	timer_object_t * test = timer_root;
	timer_object_t * test_prev = timer_root;
	while(test != NULL)
	{
		if(t->ticks <= test->ticks) // 100 < 1000
		{
									// A->B->TEST_PREV->TEST->E

			t->next = test; 		// A->B->TEST_PREV->TEST->E
									//           T------>/

			test_prev->next = t;	// A->B->TEST_PREV->T->TEST->E
			active_timers++;

			t->ticks = test->ticks - t->ticks;
			return 0;
		}
		if(test->next == NULL)
		{
			break;
		}
		test_prev = test;
		test = test->next;
	}
	//PROBABLY a longggggggggg running task, appending to list
	test_prev->next = t;
	t->next = NULL;
	return 0;
}

void kernel_timer_reaper(timer_object_t * t)
{
	printk("test","REAPER!\n");
}

void cycle_timers()
{
	if(timer_root == NULL)
	{
		return;
	}

	if(timer_root->ticks == 0)
	{
		timer_root->handler(timer_root);
		if(timer_root->repeat == 0)
		{
			timer_root = timer_root->next;
		}
		else
		{
			timer_object_t * timer = timer_root;
			timer_root = timer_root->next;
			timer->ticks = timer->repeat;
			insert_timer(timer);
		}
		return;
	}
	else
	{
		timer_root->ticks--;
	}
}

void test_timer()
{
	printk("testA","Called!\n");
}

int init_timer()
{
	timer_default = create_timer("ktreaperd",1000,1000,kernel_timer_reaper);
	timer_root = timer_default;
	active_timers++;
	timer_object_t * test = create_timer("test",100,100,test_timer);
	insert_timer(test);
	return 0;
}