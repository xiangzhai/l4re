INTERFACE [mips32]:

// empty: should generate a linker error when used
bool cas2_unsafe(Mword *, Mword *, Mword *);

IMPLEMENTATION [mips32]:

// ``unsafe'' stands for no safety according to the size of the given type.
// There are type safe versions of the cas operations in the architecture
// independent part of atomic that use the unsafe versions and make a type
// check.

inline
bool
cas_unsafe(Mword *ptr, Mword oldval, Mword newval)
{
  Mword ret;

  __asm__ __volatile__(
      "     .set    push                \n"
      "     .set    noat    #CAS        \n"
      "     .set    noreorder           \n"
      "1:   lw      %[ret], %[ptr]      \n"
      "     bne     %[ret], %z[old], 2f \n"
      "       nop                       \n"
      "     ll      %[ret], %[ptr]      \n"
      "     bne     %[ret], %z[old], 2f \n"
      "       move    $1, %z[newval]    \n"
      "     sc      $1, %[ptr]          \n"
      "     beqz    $1, 1b              \n"
      "       nop                       \n"
      "     .set    pop                 \n"
      "2:                               \n"
      : [ret] "=&r" (ret), [ptr] "+ZC" (*ptr)
      : [old] "Jr" (oldval), [newval] "Jr" (newval));

  // true is ok
  // false is failed
  return ret == oldval;
}

inline
void
atomic_and(Mword *l, Mword mask)
{
  Mword tmp;

  do
    {
      __asm__ __volatile__(
          "ll      %[tmp], %[ptr]  \n"
          "and     %[tmp], %[mask] \n"
          "sc      %[tmp], %[ptr]  \n"
          : [tmp] "=&r" (tmp), [ptr] "+ZC" (*l)
          : [mask] "Ir" (mask));
    }
  while (!tmp);
}

inline
void
atomic_or(Mword *l, Mword bits)
{
  Mword tmp;

  do
    {
      __asm__ __volatile__(
          "ll  %[tmp], %[ptr]  \n"
          "or  %[tmp], %[bits] \n"
          "sc  %[tmp], %[ptr]  \n"
          : [tmp] "=&r" (tmp), [ptr] "+ZC" (*l)
          : [bits] "Ir" (bits));
    }
  while (!tmp);
}

inline
void
atomic_add(Mword *l, Mword v)
{
  Mword tmp;

  do
    {
      __asm__ __volatile__(
          "ll   %[tmp], %[ptr]  \n"
          "addu %[tmp], %[v] \n"
          "sc   %[tmp], %[ptr]  \n"
          : [tmp] "=&r" (tmp), [ptr] "+ZC" (*l)
          : [v] "Jg" (v));
    }
  while (!tmp);
}

//---------------------------------------------------------------------------
IMPLEMENTATION [mips32 && mp]:

#include "mem.h"

// empty: should generate a linker error when used
template<typename W> inline
bool mp_cas2_arch(void *, W, Mword, Mword, Mword)
{
  W::no_cas2 = 10; // trigger a compile error if cas2 is used
  return false;
}

inline NEEDS["mem.h"]
void
atomic_mp_and(Mword *l, Mword value)
{
  Mem::mp_mb();
  atomic_and(l, value);
  Mem::mp_mb();
}

inline NEEDS["mem.h"]
void
atomic_mp_or(Mword *l, Mword value)
{
  Mem::mp_mb();
  atomic_or(l, value);
  Mem::mp_mb();
}

inline NEEDS["mem.h"]
void
atomic_mp_add(Mword *l, Mword value)
{
  Mem::mp_mb();
  atomic_add(l, value);
  Mem::mp_mb();
}

inline NEEDS["mem.h"]
bool
mp_cas_arch(Mword *m, Mword o, Mword n)
{
  Mem::mp_mb();
  Mword ret = cas_unsafe(m, o, n);
  Mem::mp_mb();
  return ret;
}

//---------------------------------------------------------------------------
IMPLEMENTATION [mips32 && !mp]:

inline
void
atomic_mp_and(Mword *l, Mword value)
{ atomic_and(l, value); }

inline
void
atomic_mp_or(Mword *l, Mword value)
{ atomic_or(l, value); }

inline
bool
mp_cas_arch(Mword *m, Mword o, Mword n)
{ return cas_unsafe(m, o, n); }

