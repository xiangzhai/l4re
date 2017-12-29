IMPLEMENTATION [pf_rk3288 && (!(arm_mpcore || arm_v7 || arm_v8)
                                || pf_rk3288_pbx)]:

IMPLEMENT int Uart::irq() const { return 44; }

IMPLEMENTATION [pf_rk3288 && (arm_mpcore || arm_v7)
                && !pf_rk3288_pbx && !pf_rk3288_vexpress]:

IMPLEMENT int Uart::irq() const { return 36; }

IMPLEMENTATION [pf_rk3288 && pf_rk3288_vexpress]:

IMPLEMENT int Uart::irq() const { return 37; }

IMPLEMENTATION: // --------------------------------------------------------

#include "mem_layout.h"
#include "uart_pl011.h"

IMPLEMENT Address Uart::base() const { return Mem_layout::Uart_phys_base; }

IMPLEMENT L4::Uart *Uart::uart()
{
  static L4::Uart_pl011 uart(24019200);
  return &uart;
}
