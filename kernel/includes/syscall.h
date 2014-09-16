#ifndef SYSCALL_H
#define SYSCALL_H
/// Generic Syscall Handler
typedef void (*syscall_handler_t)(uint32_t,...);

void register_syscall_handler (uint8_t n, syscall_handler_t h);
void deregister_syscall_handler (uint8_t n);

#define MAX_SYSCALLS 0xFF

#define SYSCALL_PANIC 0x0
#define SYSCALL_OOPS 0x1
#endif