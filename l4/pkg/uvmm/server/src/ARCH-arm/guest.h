/*
 * Copyright (C) 2015 Kernkonzept GmbH.
 * Author(s): Sarah Hoffmann <sarah.hoffmann@kernkonzept.com>
 *
 * This file is distributed under the terms of the GNU General Public
 * License, version 2.  Please see the COPYING-GPL-2 file for details.
 */
#pragma once

#include <l4/cxx/ref_ptr>

#include "device.h"
#include "generic_guest.h"
#include "gic.h"
#include "vcpu.h"

namespace Vmm {

/**
 * ARM virtual machine monitor.
 */
class Guest : public Generic_guest
{
public:
  Guest(L4::Cap<L4Re::Dataspace> ram,
        l4_addr_t vm_base = ~0UL);

  void load_device_tree(char const *name)
  { load_device_tree_at(name, 0x100, 0x200); }

  l4_addr_t load_linux_kernel(char const *kernel, char const *cmd_line, Cpu vcpu);

  void run(Cpu vcpu);

  l4_msgtag_t handle_entry(Cpu vcpu);

  static Guest *create_instance(L4::Cap<L4Re::Dataspace> ram);

  void show_state_registers() override;
  void show_state_interrupts() override;

  int config_as_core_device(L4::Cap<L4Re::Dataspace> iods,
                            Vdev::Dt_node const &node,
                            cxx::Ref_ptr<Vdev::Device> *dev);

private:
  cxx::Ref_ptr<Gic::Dist> _gic;
};

} // namespace
