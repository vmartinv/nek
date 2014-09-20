/*
 * This header contains definition of the various standard C error codes.
 *
 * Functions should adhere to returning these, as kernel functions may return
 * such codes, and it obviously makes the code cleaner to have one unified
 * set of error codes.
 */

#ifndef ERRNO_H
#define ERRNO_H

#define ENONE 0
#define ERANGE 1
#define ENOTFOUND 2
#define EBUSY 3
#define EINVAL 4
#define EDOM 5

// Global symbol indicating last error
extern int errno;

void panic(char *s);
void oops(char *s);

#endif
