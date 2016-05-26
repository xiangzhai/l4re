/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License 2.  See the file "COPYING-GPL-2" in the main directory of this
 * archive for more details.
 *
 * Copyright (C) 2013 Imagination Technologies Ltd.
 * Author: Yann Le Du <ledu@kymasys.com>
 */

.section .text.init, "ax"
.type __start, @function
.globl __start
.set noreorder
__start:
	.cpload $25 /* load GP */
	la    $29, crt0_stack_high
	j     init
	  subu  $29, 6 * 4
1:
	b   1b

.section ".bss", "aw"
.align 12
.global crt0_stack_low
crt0_stack_low:
	.space 4096
.global crt0_stack_high
crt0_stack_high:
