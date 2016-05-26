/*
 * Copyright (C) 2014 Imagination Technologies Ltd.
 * Author: Sanjay Lal <sanjayl@kymasys.com>
 * Author: Yann Le Du <ledu@kymasys.com>
 */

IMPLEMENTATION [mips32]:

#include "warn.h"
#include "types.h"
#include "cp0_status.h"
#include "alternatives.h"

/// Unblock external inetrrupts
IMPLEMENT static inline ALWAYS_INLINE
void Proc::sti()
{
  asm volatile (
    "ei   \n"
    "ehb  \n"
    : /* no outputs */
    : /* no inputs */
    : "memory"
  );

}

/// Block external interrupts
IMPLEMENT static inline
void Proc::cli()
{
   asm volatile (
    "di   \n"
    "ehb  \n"
    : /* no outputs */
    : /* no inputs */
    : "memory"
  );
}

/// Are external interrupts enabled ?
IMPLEMENT static inline NEEDS["cp0_status.h"]
Proc::Status
Proc::interrupts()
{
  return (Status)Cp0_status::read() & Cp0_status::ST_IE;
}

/// Are interrupts enabled in saved status state?
PUBLIC static inline NEEDS["cp0_status.h"]
Proc::Status
Proc::interrupts(Status status)
{
  return status & Cp0_status::ST_IE;
}

/// Block external interrupts and save the old state
IMPLEMENT static inline ALWAYS_INLINE NEEDS["cp0_status.h"]
Proc::Status
Proc::cli_save()
{
  Status flags;

   asm volatile (
    "di   %[flags]\n"
    "ehb  \n"
    : [flags] "=r" (flags)
    : /* no inputs */
    : "memory"
  );
  return flags & Cp0_status::ST_IE;
}

/// Conditionally unblock external interrupts
IMPLEMENT static inline ALWAYS_INLINE NEEDS["cp0_status.h"]
void
Proc::sti_restore(Status status)
{
  if (status & Cp0_status::ST_IE)
    Proc::sti();
}

IMPLEMENT static inline
void
Proc::pause()
{
  // FIXME: could use 'rp' here?
}

IMPLEMENT static inline NEEDS["cp0_status.h"]
void
Proc::halt()
{
  asm volatile ("wait");
}
#if 0
PUBLIC static inline
Mword Proc::wake(Mword /* srr1 */)
{
  NOT_IMPL_PANIC;
}
#endif

IMPLEMENT static inline
void
Proc::irq_chance()
{
  asm volatile ("nop; nop;" : : :  "memory");
}

IMPLEMENT static inline
void
Proc::stack_pointer(Mword sp)
{
  asm volatile ("move $29,%0" : : "r" (sp));
}

IMPLEMENT static inline
Mword
Proc::stack_pointer()
{
  Mword sp;
  asm volatile ("move %0,$29" : "=r" (sp));
  return sp;
}

IMPLEMENT static inline
Mword
Proc::program_counter()
{
  Mword pc;
  asm volatile (
      "move  $t0, $ra  \n"
      "jal   1f        \n"
      "1:              \n"
      "move  %0, $ra   \n"
      "move  $ra, $t0  \n"
      : "=r" (pc) : : "t0");
  return pc;
}

PUBLIC static inline
void
Proc::cp0_exec_hazard()
{ __asm__ __volatile__ ("ehb"); }

PUBLIC static inline
Mword
Proc::get_ulr()
{
  Mword v;
  __asm__ __volatile__ ("mfc0 %0, $4, 2" : "=r"(v));
  return v;
}

PUBLIC static inline NEEDS["alternatives.h"]
void
Proc::set_ulr(Mword ulr)
{
  __asm__ __volatile__ (ALTERNATIVE_INSN(
        "nop",
        "mtc0 %0, $4, 2", /* load ULR if it is supported */
        0x00000002        /* feature bit 1 (see Cpu::Options ulr) */
        )
      : : "r"(ulr));
}

IMPLEMENTATION [mips32 && !mp]:

PUBLIC static inline
Cpu_phys_id
Proc::cpu_id()
{ return Cpu_phys_id(0); }

IMPLEMENTATION [mips32 && mp]:

PUBLIC static inline
Cpu_phys_id
Proc::cpu_id()
{
  Mword v;
  asm volatile ("mfc0 %0, $15, 1" : "=r"(v));
  return Cpu_phys_id(v & 0x3ff);
}

