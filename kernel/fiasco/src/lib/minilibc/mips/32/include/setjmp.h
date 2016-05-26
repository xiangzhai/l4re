#pragma once

#include <cdefs.h>

typedef int __jmp_buf[12];

typedef struct __jmp_buf_struct
{
  /* Program counter.  */
  int __pc;

  /* Stack pointer.  */
  int __sp;

  /* Callee-saved registers s0 through s7.  */
  int __regs[8];

  /* The frame pointer.  */
  int __fp;

  /* The global pointer.  */
  int __gp;
} jmp_buf[1];

__BEGIN_DECLS

void longjmp (jmp_buf __env, int __val)
     __attribute__ ((__noreturn__));

int setjmp(jmp_buf env);

__END_DECLS
