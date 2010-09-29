/*
 * (c) 2010 Adam Lackorzynski <adam@os.inf.tu-dresden.de>
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

#include <l4/vcpu/vcpu.h>
#include <stdio.h>

void l4vcpu_print_state(l4_vcpu_state_t *vcpu,
                        const char *prefix) L4_NOTHROW
{
  printf("%svcpu=%p state=%lx savedstate=%lx label=%lx\n",
         prefix, vcpu, vcpu->state, vcpu->saved_state, vcpu->i.label);
  printf("%ssticky=%lx user_task=%lx\n",
         prefix, vcpu->sticky_flags, vcpu->user_task << L4_CAP_SHIFT);
  printf("%sentry_sp=%lx entry_ip=%lx\n",
         prefix, vcpu->entry_sp, vcpu->entry_ip);
  l4vcpu_print_state_arch(vcpu, prefix);
}
