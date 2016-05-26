/*
 * (c) 2009 Adam Lackorzynski <adam@os.inf.tu-dresden.de>
 *     economic rights: Technische Universität Dresden (Germany)
 *
 * This file is part of TUD:OS and distributed under the terms of the
 * GNU General Public License 2.
 * Please see the COPYING-GPL-2 file for details.
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 */
#pragma once

#include <l4/sys/compiler.h>

L4_INLINE long
l4_atomic_add(volatile long *mem, long offset) L4_NOTHROW
{
  long ret;
  __asm__ __volatile__ (
      ".set push                           \n\t"
      ".set noat                           \n\t"
      ".set reorder                        \n\t"
      "1: ll   %[ret], %[mem]              \n\t"
      "   addu $1, %[ret], %[offset]       \n\t"
      "   sc   $1, %[mem]                  \n\t"
      "   beqz $1, 1b                      \n\t"
      ".set pop                            \n\t"
  : [ret] "=&r" (ret), [mem] "+ZC" (*mem)
  : [offset] "Jg" (offset) );

  return ret + offset;
}

L4_INLINE long
l4_atomic_cmpxchg(volatile long *mem, long oldval, long newval) L4_NOTHROW
{
  long ret;
  long tmp;

  __asm__ __volatile__ (
      ".set push                   \n\t"
      ".set noat                   \n\t"
      ".set noreorder              \n\t"
      "   addiu %[ret], $0, 0      \n\t"
      "1: lw    %[tmp], %[mem]     \n\t"
      "   bne   %[tmp], %[old], 2f \n\t"
      "     nop                    \n\t"
      "   ll   %[tmp], %[mem]      \n\t"
      "   bne  %[tmp], %[old], 2f  \n\t"
      "     add  $1, %[val], $0    \n\t"
      "   sc   $1, %[mem]          \n\t"
      "   beqz $1, 1b              \n\t"
      "     nop                    \n\t"
      "   addiu %[ret], $0, 1      \n\t"
      "2:                          \n\t"
      ".set pop                    \n\t"
  : [tmp] "=&r"(tmp), [ret] "=&r" (ret), [mem] "+ZC" (*mem)
  : [val] "r" (newval), [old] "r" (oldval)
  : "memory");

  return ret;
}

L4_INLINE long
l4_atomic_xchg(volatile long *mem, long newval) L4_NOTHROW
{
  long ret;

  __asm__ __volatile__ (
      ".set push                \n\t"
      ".set noat                \n\t"
      ".set reorder             \n\t"
      "1: ll   %[ret], %[mem]   \n\t"
      "   addiu $1, %[val], 0   \n\t"
      "   sc   $1, %[mem]       \n\t"
      "   beqz $1, 1b           \n\t"
      ".set pop                 \n\t"
  : [ret] "=&r" (ret), [mem] "+ZC" (*mem)
  : [val] "r" (newval)
  : "memory");

  return ret;
}
