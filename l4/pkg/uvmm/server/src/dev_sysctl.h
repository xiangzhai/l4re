/*
 * Copyright (C) 2016 Kernkonzept GmbH.
 * Author(s): Sarah Hoffmann <sarah.hoffmann@kernkonzept.com>
 *
 * This file is distributed under the terms of the GNU General Public
 * License, version 2.  Please see the COPYING-GPL-2 file for details.
 */
#pragma once

#include <cstdlib>

#include "arch_mmio_device.h"
#include "debug.h"
#include "device.h"

namespace Vdev {

/**
 * A simple system controller with the following functions:
 *
 *   0x00 - On write exit with the given value as exit code.
 */
struct System_controller : public Device
{
  void init_device(Device_lookup const &, Dt_node const &) override {}

  l4_uint32_t read(unsigned, char, unsigned)
  { return 0; }

  void write(unsigned reg, char, l4_uint32_t value, unsigned)
  {
    switch (reg)
      {
      case 0:
        Dbg().printf("Shutdown (%d) requested\n", value);
        exit(value);
      }
  }
};

struct System_controller_mmio
: public System_controller,
  public Vmm::Mmio_device_t<System_controller_mmio>
{};


} // namespace
