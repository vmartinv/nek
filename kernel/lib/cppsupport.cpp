#include <stddef.h>
#include <lib/liballoc.h>

void *operator new(size_t size){
    return malloc(size);}
 
void *operator new[](size_t size){
    return malloc(size);}
 
void operator delete(void *p){
    free(p);}
 
void operator delete[](void *p){
    free(p);}



extern "C" void __cxa_pure_virtual(){
    // Do nothing or print an error message.
}
