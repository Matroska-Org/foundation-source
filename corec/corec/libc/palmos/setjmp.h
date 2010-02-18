#ifndef __SETJMP_H_
#define __SETJMP_H_

// for MatroskaParser.c
#ifndef EOF
#define EOF -1
#endif

// for lua
#define EXIT_SUCCESS    0
#define EXIT_FAILURE    1
void exit( int status );

typedef int jmp_buf[16];
int setjmp(jmp_buf jp);
int longjmp(jmp_buf jp, int ret);

#endif
