#pragma once

inline void
switch_stack(unsigned long stack, void (*func)())
{
  register unsigned long v0 asm("v0") = 0;
  asm volatile ( ".set push            \n\t"
                 ".set noreorder       \n\t"
                 "jr   %[func]         \n\t"
                 "  move $sp, %[stack] \n\t"
                 ".set pop             \n\t"
		 : : [stack] "r" (stack), [func] "r" (func), "r" (v0)
		 : "memory");
}

