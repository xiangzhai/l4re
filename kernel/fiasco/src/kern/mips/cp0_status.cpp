/*
 * Copyright (C) 2014 Imagination Technologies Ltd.
 * Author: Sanjay Lal <sanjayl@kymasys.com>
 * Author: Yann Le Du <ledu@kymasys.com>
 */

INTERFACE:

#include "types.h"

class Cp0_status
{
public:
  enum Status_bits
  {
    ST_IE             = 1UL << 0,
    ST_EXL            = 1UL << 1,
    ST_ERL            = 1UL << 2,
    ST_KSU_MASK       = 3UL << 3,
    ST_KSU_SUPERVISOR = 1UL << 3,
    ST_KSU_USER       = 1UL << 4,
    ST_UX             = 1UL << 5,
    ST_SX             = 1UL << 6,
    ST_KX             = 1UL << 7,
    ST_IP             = 0xFFUL << 8,
    ST_IP0            = 1UL << 8,
    ST_IP1            = 1UL << 9,
    ST_IP2            = 1UL << 10,
    ST_IP3            = 1UL << 11,
    ST_IP4            = 1UL << 12,
    ST_IP5            = 1UL << 13,
    ST_IP6            = 1UL << 14,
    ST_IP7            = 1UL << 15,
    ST_DE             = 1UL << 16,
    ST_CE             = 1UL << 17,
    ST_FR             = 1UL << 26,
    ST_CU0            = 1UL << 28,
    ST_CU1            = 1UL << 29,
  };
};

//------------------------------------------------------------------------------
IMPLEMENTATION:

PUBLIC static inline
Mword
Cp0_status::read()
{
  Mword r;
  __asm__ __volatile__ ("mfc0  %0, $12" : "=r"(r));
  return r;
}

PUBLIC static inline
void
Cp0_status::write(Mword v)
{
  __asm__ __volatile__ ("mtc0  %z0, $12" : : "Jr"(v));
}

PRIVATE static inline
void
Cp0_status::check_status_hazards(Mword bits)
{
  //for these context synchronization is mandatory
  enum { mask = ST_IE | ST_CU1 };

  if(bits & mask)
    asm volatile ("ehb");
}

PUBLIC static inline NEEDS[Cp0_status::check_status_hazards]
void
Cp0_status::set_status_bit(Mword bits)
{
  Mword reg = read();
  reg |= bits;
  write(reg);
  check_status_hazards(bits);
}

PUBLIC static inline NEEDS[Cp0_status::check_status_hazards]
void
Cp0_status::clear_status_bit(Mword bits)
{
  Mword reg = read();
  reg &= ~bits;
  write(reg);
  check_status_hazards(bits);
}

/*
 * set user mode, enable interrupts and interrupt masks, and set EXL to prepare
 * for eret to user mode
 */
PUBLIC static inline
Mword
Cp0_status::status_eret_to_user_ei(Mword status_word)
{
  status_word &= ~ST_KSU_MASK;
  status_word |= ST_KSU_USER | ST_EXL | ST_IE;
  return status_word;
}

