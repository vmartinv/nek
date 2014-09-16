#ifndef TASK_H
#include <stddef.h>
#include <stdint.h>
#define TASK_STATE_STOPPED		0x0 // Task is not executing (and won't)
#define TASK_STATE_ACTIVE		0x1 // Task is executing
#define TASK_STATE_BLOCKED		0x2 // Task is waiting for a resource
#define TASK_STATE_WAITING		0x3 // Task is waiting for execution. (and will as soon as the scheduler selects it for execution)
#define TASK_STATE_TERMINATED 	0x4 // Task is to be removed as soon as the scheduler gets to it.
typedef struct context
{
	uint32_t eax, ebx, ecx, edx, esi, edi;
	uint32_t stack_base, stack;
	uint32_t eip;
	uint32_t eflags;
} context_t;

struct task;
typedef struct thread
{
	uint32_t id;
	struct task * parent;
	context_t * context;
} thread_t;
typedef struct task
{
	uint32_t id;
	uint8_t state;
	uint32_t port;
	uint32_t owner;
	thread_t * main_thread;
	struct task * parent; //NULL if 
} task_t;
#endif