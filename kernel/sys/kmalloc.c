#include <types.h>
#include <sys/memory.h>
#include <sys/logging.h>
#include <string.h>

extern uint32_t _kernel_end;
static uintptr_t pre_placementaddr;
static uint32_t kmalloc_status = 1;

#ifdef BOARDx86generic

uintptr_t kmalloc_get_pre_placementaddr(){
	return pre_placementaddr;
}

void init_kmalloc(uintptr_t kernelend)
{
	pre_placementaddr = kernelend;
	kmalloc_status = 0;
}
void disable_kmalloc()
{
	kmalloc_status=1;
}

uintptr_t * kmalloc(size_t sz)
{
	if(kmalloc_status == 0){
		uint32_t tmp = pre_placementaddr;
		pre_placementaddr += sz;
		return (uintptr_t *)tmp;
	}
	return NULL;
}


#endif
