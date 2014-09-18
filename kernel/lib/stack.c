
#include <lib/stack.h>

// Allocation/deallocation functions
inline stack* stack_create(){return list_create();}
inline void stack_destroy(stack*s){list_destroy(s);}

inline void stack_push(stack *s, void* data){list_append(s, list_create_item(data));}
inline void stack_pop(stack *s){list_pop(s);}
inline void* stack_top(stack *s){return s->tail->value;}
