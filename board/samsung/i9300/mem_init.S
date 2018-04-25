/*
 * (C) Copyright 2011 Samsung Electronics Co. Ltd
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <config.h>
#include <asm/arch/cpu.h>
#define ELFIN_CLOCK_BASE 0x10030000
#define CLK_SRC_DMC_OFFSET		0x10200
#define CLK_DIV_DMC0_OFFSET		0x10500
#define CLK_DIV_DMC1_OFFSET		0x10504
#define APB_DMC_0_BASE			0x10600000
#define APB_DMC_1_BASE			0x10610000

#define DMC_CONCONTROL			0x00
#define DMC_MEMCONTROL			0x04
#define DMC_MEMCONFIG0			0x08
#define DMC_MEMCONFIG1			0x0C
#define DMC_DIRECTCMD			0x10
#define DMC_PRECHCONFIG		0x14
#define DMC_PHYCONTROL0		0x18
#define DMC_PHYCONTROL1		0x1C
#define DMC_PHYCONTROL2		0x20
#define DMC_PWRDNCONFIG		0x28
#define DMC_TIMINGAREF			0x30
#define DMC_TIMINGROW			0x34
#define DMC_TIMINGDATA			0x38
#define DMC_TIMINGPOWER		0x3C
#define DMC_PHYSTATUS			0x40
#define DMC_PHYZQCONTROL		0x44
#define DMC_CHIP0STATUS		0x48
#define DMC_CHIP1STATUS		0x4C
#define DMC_AREFSTATUS			0x50
#define DMC_MRSTATUS			0x54
#define DMC_PHYTEST0			0x58
#define DMC_PHYTEST1			0x5C
#define DMC_QOSCONTROL0		0x60
#define DMC_QOSCONFIG0			0x64
#define DMC_QOSCONTROL1		0x68
#define DMC_QOSCONFIG1			0x6C
#define DMC_QOSCONTROL2		0x70
#define DMC_QOSCONFIG2			0x74
#define DMC_QOSCONTROL3		0x78
#define DMC_QOSCONFIG3			0x7C
#define DMC_QOSCONTROL4		0x80
#define DMC_QOSCONFIG4			0x84
#define DMC_QOSCONTROL5		0x88
#define DMC_QOSCONFIG5			0x8C
#define DMC_QOSCONTROL6		0x90
#define DMC_QOSCONFIG6			0x94
#define DMC_QOSCONTROL7		0x98
#define DMC_QOSCONFIG7			0x9C
#define DMC_QOSCONTROL8		0xA0
#define DMC_QOSCONFIG8			0xA4
#define DMC_QOSCONTROL9		0xA8
#define DMC_QOSCONFIG9			0xAC
#define DMC_QOSCONTROL10		0xB0
#define DMC_QOSCONFIG10		0xB4
#define DMC_QOSCONTROL11		0xB8
#define DMC_QOSCONFIG11		0xBC
#define DMC_QOSCONTROL12		0xC0
#define DMC_QOSCONFIG12		0xC4
#define DMC_QOSCONTROL13		0xC8
#define DMC_QOSCONFIG13		0xCC
#define DMC_QOSCONTROL14		0xD0
#define DMC_QOSCONFIG14		0xD4
#define DMC_QOSCONTROL15		0xD8
#define DMC_QOSCONFIG15		0xDC
#define DMC_IVCONTROL			0xF0

#define MEM_DLL
#define MCLK_400


	.globl mem_asm_init
mem_asm_init:

	/* CLK_DIV_DMC0 on iROM DMC=50MHz for Init DMC */
	ldr	r0, =ELFIN_CLOCK_BASE	@0x1003_0000
	ldr	r1, =0x00117713
	ldr	r2, =CLK_DIV_DMC0_OFFSET
	str	r1, [r0, r2]

/*****************************************************************/
/*DREX0***********************************************************/
/*****************************************************************/

	ldr	r0, =APB_DMC_0_BASE

	ldr	r1, =0xE3855403
	str	r1, [r0, #DMC_PHYZQCONTROL]

	ldr	r1, =0x71101008
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0x7110100A
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0x00000084
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x71101008
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0x0000008C
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x00000084
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x0000008C
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x00000084
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x0FFF30CA
	str	r1, [r0, #DMC_CONCONTROL]

	ldr	r1, =0x00202500
	str	r1, [r0, #DMC_MEMCONTROL]

	ldr	r1, =0x40C01323
	str	r1, [r0, #DMC_MEMCONFIG0]

	ldr	r1, =(0x80000007)
	str	r1, [r0, #DMC_IVCONTROL]

#ifdef CONFIG_EVT0_RECOMMEND
	ldr	r1, =0x6400ffff
#else
	ldr	r1, =0x64000000
#endif
	str	r1, [r0, #DMC_PRECHCONFIG]

	ldr	r1, =0x9C4000FF
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0x0000005D
	str	r1, [r0, #DMC_TIMINGAREF] @TimingAref

#ifdef MCLK_330
	ldr	r1, =0x2b47654e
	str	r1, [r0, #DMC_TIMINGROW]
	ldr	r1, =0x35330306
	str	r1, [r0, #DMC_TIMINGDATA]
	ldr	r1, =0x442f0365
	str	r1, [r0, #DMC_TIMINGPOWER]
#else
#ifdef CONFIG_EVT0_RECOMMEND
	ldr	r1, =0x34A98691
#else
	ldr	r1, =0x34498691
#endif
	str	r1, [r0, #DMC_TIMINGROW] @TimingRow
	ldr	r1, =0x36330306
	str	r1, [r0, #DMC_TIMINGDATA] @TimingData
	ldr	r1, =0x50380365
	str	r1, [r0, #DMC_TIMINGPOWER] @TimingPower
#endif

	mov	r2, #0x100000
2:	subs	r2, r2, #1
	bne	2b

	ldr	r1, =0x07000000
	str	r1, [r0, #DMC_DIRECTCMD]

	mov	r2, #0x100000
3:	subs	r2, r2, #1
	bne	3b

	ldr	r1, =0x00071C00
	str	r1, [r0, #DMC_DIRECTCMD]

	mov	r2, #0x100000
4:	subs	r2, r2, #1
	bne	4b

	ldr	r1, =0x00010BFC
	str	r1, [r0, #DMC_DIRECTCMD]

	mov	r2, #0x100000
5:	subs	r2, r2, #1
	bne	5b

	ldr	r1, =0x00000608
	str	r1, [r0, #DMC_DIRECTCMD]
	ldr	r1, =0x00000810
	str	r1, [r0, #DMC_DIRECTCMD]
	ldr	r1, =0x00000C08
	str	r1, [r0, #DMC_DIRECTCMD]

/*****************************************************************/
/*DREX1***********************************************************/
/*****************************************************************/
	ldr	r0, =APB_DMC_1_BASE

	ldr	r1, =0xE3855403
	str	r1, [r0, #DMC_PHYZQCONTROL]

	ldr	r1, =0x71101008
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0x7110100A
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0x00000084
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x71101008
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0x0000008C
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x00000084
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x0000008C
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr r1, =0x00000084
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x0FFF30CA
	str	r1, [r0, #DMC_CONCONTROL]

	ldr	r1, =0x00202500
	str	r1, [r0, #DMC_MEMCONTROL]

	ldr	r1, =0x40C01323
	str	r1, [r0, #DMC_MEMCONFIG0]

	ldr	r1, =(0x80000007)
	str	r1, [r0, #DMC_IVCONTROL]

#ifdef CONFIG_EVT0_RECOMMEND
	ldr	r1, =0x6400ffff
#else
	ldr	r1, =0x64000000
#endif
	str	r1, [r0, #DMC_PRECHCONFIG]

	ldr	r1, =0x9C4000FF
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0x0000005D
	str	r1, [r0, #DMC_TIMINGAREF] @TimingAref

#ifdef MCLK_330
	ldr	r1, =0x2b47654e
	str	r1, [r0, #DMC_TIMINGROW]
	ldr	r1, =0x35330306
	str	r1, [r0, #DMC_TIMINGDATA]
	ldr	r1, =0x442f0365
	str	r1, [r0, #DMC_TIMINGPOWER]
#else
#ifdef CONFIG_EVT0_RECOMMEND
	ldr	r1, =0x34A98691
#else
	ldr	r1, =0x34498691
#endif
	str	r1, [r0, #DMC_TIMINGROW] @TimingRow
	ldr	r1, =0x36330306
	str	r1, [r0, #DMC_TIMINGDATA] @TimingData
	ldr	r1, =0x50380365
	str	r1, [r0, #DMC_TIMINGPOWER] @TimingPower
#endif

	mov	r2, #0x100000
2:	subs	r2, r2, #1
	bne	2b

	ldr	r1, =0x07000000
	str	r1, [r0, #DMC_DIRECTCMD]

	mov	r2, #0x100000
3:	subs	r2, r2, #1
	bne	3b

	ldr	r1, =0x00071C00
	str	r1, [r0, #DMC_DIRECTCMD]

	mov	r2, #0x100000
4:	subs	r2, r2, #1
	bne	4b

	ldr	r1, =0x00010BFC
	str	r1, [r0, #DMC_DIRECTCMD]

	mov	r2, #0x100000
5:	subs	r2, r2, #1
	bne	5b

	ldr	r1, =0x00000608
	str	r1, [r0, #DMC_DIRECTCMD]
	ldr	r1, =0x00000810
	str	r1, [r0, #DMC_DIRECTCMD]
	ldr	r1, =0x00000C08
	str	r1, [r0, #DMC_DIRECTCMD]

	mov	pc, lr

