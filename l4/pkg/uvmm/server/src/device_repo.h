/*
 * Copyright (C) 2016 Kernkonzept GmbH.
 * Author(s): Sarah Hoffmann <sarah.hoffmann@kernkonzept.com>
 *
 * This file is distributed under the terms of the GNU General Public
 * License, version 2.  Please see the COPYING-GPL-2 file for details.
 */
#pragma once

#include <string>
#include <vector>

#include "device.h"
#include "device_tree.h"
#include "dev_sysctl.h"
#include "guest.h"
#include "virtio_console.h"
#include "virtio_proxy.h"

namespace Vdev {

class Device_repository : public Device_lookup
{
  struct Dt_device
  {
    std::string path;
    l4_uint32_t phandle;
    cxx::Ref_ptr<Device> dev;
  };

public:
  cxx::Ref_ptr<Device> device_from_node(Dt_node const &node) const override
  {
    l4_uint32_t phandle = node.get_phandle();

    if (phandle != 0 && phandle != -1U)
      {
        for (auto const &d : _devices)
          {
            if (d.phandle == phandle)
              return d.dev;
          }
      }

    char buf[1024];
    node.get_path(buf, sizeof(buf));

    for (auto const &d : _devices)
      {
        if (d.path == buf)
          return d.dev;
      }

    return cxx::Ref_ptr<Device>();
  }

  void add(char const *path, l4_uint32_t phandle, cxx::Ref_ptr<Device> dev)
  { _devices.push_back({path, phandle, dev}); }

  void init_devices(Device_tree dt)
  {
    for (auto &d : _devices)
      {
        Dbg().printf("Init device '%s'.\n", d.path.c_str());

        auto node = dt.invalid_node();
        if (d.phandle != 0 && d.phandle != -1U)
          node = dt.phandle_offset(d.phandle);

        if (!node.is_valid())
          node = dt.path_offset(d.path.c_str());

        d.dev->init_device(*this, node);
      }
  }

  int config_as_virtio_device(Vmm::Guest *vmm, Dt_node const &node,
                              cxx::Ref_ptr<Device> *dev)
    {
    if (node.is_compatible("virtio,mmio"))
      return 0;

    int type_len;
    auto *type = node.get_prop<char>("l4vmm,virtiotype", &type_len);
    if (!type)
      {
        Err().printf("'l4vmm,virtiotype' property missing from virtio device.\n");
        return -L4_ENODEV;
      }

    if (fdt_stringlist_contains(type, type_len, "console"))
      {
        Dbg().printf("Create virtual console\n");

        auto cons =
          Vdev::make_device<Virtio_console_mmio>(&vmm->ram(),
                                                 L4Re::Env::env()->log());
        cons->register_obj(vmm->registry());
        *dev = cons;

        vmm->register_mmio_device(std::move(cons), node);
        return 1;
      }

    if (fdt_stringlist_contains(type, type_len, "net"))
      {
        Dbg().printf("Create virtual net\n");

        auto cap = L4Re::Env::env()->get_cap<L4virtio::Device>("net");
        if (!cap)
          {
            Dbg(Dbg::Warn).printf("Network switch not found.\n");
            return -L4_ENODEV;
          }

        auto &ram = vmm->ram();
        auto proxy = Vdev::make_device<Vdev::Virtio_proxy_mmio>(&ram);
        proxy->register_obj(vmm->registry(), cap, ram.ram(), ram.vm_start());
        *dev = proxy;

        vmm->register_mmio_device(std::move(proxy), node);
        return 1;
      }

    Err().printf("unknown virtio device type: '%.*s'\n", type_len, type);
    return -L4_ENODEV;
  }

  int config_as_syscon(Vmm::Guest *vmm, Dt_node const &node,
                       cxx::Ref_ptr<Device> *dev)
  {
    if (node.is_compatible("syscon-l4vmm"))
      return 0;

    auto syscon = Vdev::make_device<Vdev::System_controller_mmio>();
    *dev = syscon;

    vmm->register_mmio_device(std::move(syscon), node);

    return 1;
  }

private:
  std::vector<Dt_device> _devices;
};

} // namespace

