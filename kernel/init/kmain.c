#include <logging.h>
#include <stdio.h>
#include <vfs.h>
#include <cedille.h>

void profiler_memory();

int init_syscalls();
int task_init();
int init_timer();

int start_service(char *daemon,int essential,int (*func)())
{
	
	if((*func)()!=0)
	{
		printk("daemon","Starting kernel daemon %s...",daemon); printf(" \tfailed\n");
		if(essential)
		{
			panic("Couldn't start required daemon!");
		}
		return 1;
	}
	else
	{
		 printk("daemon","Starting kernel daemon %s...",daemon); printf(" \tdone\n");
	}
	return 0;
}

void kmain()
{
	printk("ok","Entering Kernel Enviorment...\n");
	//Start services
	start_service("syscalld",1,init_syscalls);
	start_service("taskd",1,task_init);
	start_service("kvfsd",0,init_vfs);
	start_service("timerd",1,init_timer);
	printk("login","Login unimplemented; idling main kernel thread\n");
	//profiler_memory();
	for(;;)
	{

	}
}
