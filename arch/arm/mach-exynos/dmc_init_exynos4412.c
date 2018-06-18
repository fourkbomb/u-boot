#include <common.h>
#include <config.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/cpu.h>
#include <asm/arch/dmc.h>
#include <asm/arch/power.h>
#include <debug_uart.h>
#include "common_setup.h"
#include "exynos4412_setup.h"

#define NR_TZASC_BANKS 4

/* Allow non-secure and secure access to all memory */
#define RA0_VAL 0xf0000000

static void tzasc_init(void) {
	unsigned int start = samsung_get_base_dmc_tzasc();
	unsigned int end = start + (DMC_OFFSET * (NR_TZASC_BANKS - 1));

	for (; start <= end; start += DMC_OFFSET) {
		struct exynos4412_tzasc *asc = (struct exynos4412_tzasc *)start;
		writel(RA0_VAL, &asc->region_attributes_0);
	}
}

void mem_ctrl_init(int reset)
{
	struct exynos4_dmc *dmc = (struct exynos4_dmc *)samsung_get_base_dmc_ctrl();
	struct exynos4_dmc *dmc1 = (struct exynos4_dmc *)(samsung_get_base_dmc_ctrl() + DMC_OFFSET);

	writel(0xe3855403, &dmc->phyzqcontrol);
	writel(0x71101008, &dmc->phycontrol0);
	writel(0x7110100a, &dmc->phycontrol0);

	writel(0x00000084, &dmc->phycontrol1);
	writel(0x71101008, &dmc->phycontrol0);
	writel(0x0000008c, &dmc->phycontrol1);
	writel(0x00000084, &dmc->phycontrol1);
	writel(0x0000008c, &dmc->phycontrol1);
	writel(0x00000084, &dmc->phycontrol1);

	writel(0x0fff30ca, &dmc->concontrol);
	writel(0x00202500, &dmc->memcontrol);
	writel(0x40c01323, &dmc->memconfig0);
	writel(0x80000000 | 0x7, &dmc->ivcontrol);
	writel(0x64000000, &dmc->prechconfig);
	writel(0x9c4000ff, &dmc->phycontrol0);

	writel(0x5d, &dmc->timingref);

	writel(0x34498691, &dmc->timingrow);
	writel(0x36330306, &dmc->timingdata);
	writel(0x50380365, &dmc->timingpower);

	sdelay(0x100000);

	writel(0x07000000, &dmc->directcmd);

	sdelay(0x100000);

	writel(0x00071c00, &dmc->directcmd);

	sdelay(0x100000);

	writel(0x00010bfc, &dmc->directcmd);

	sdelay(0x100000);

	writel(0x608, &dmc->directcmd);
	writel(0x810, &dmc->directcmd);
	writel(0xc08, &dmc->directcmd);

	writel(0xe3855403, &dmc1->phyzqcontrol);
	writel(0x71101008, &dmc1->phycontrol0);
	writel(0x7110100a, &dmc1->phycontrol0);

	writel(0x00000084, &dmc1->phycontrol1);
	writel(0x71101008, &dmc1->phycontrol0);
	writel(0x0000008c, &dmc1->phycontrol1);
	writel(0x00000084, &dmc1->phycontrol1);
	writel(0x0000008c, &dmc1->phycontrol1);
	writel(0x00000084, &dmc1->phycontrol1);

	writel(0x0fff30ca, &dmc1->concontrol);
	writel(0x00202500, &dmc1->memcontrol);
	writel(0x40c01323, &dmc1->memconfig0);
	writel(0x80000000 | 0x7, &dmc1->ivcontrol);
	writel(0x64000000, &dmc1->prechconfig);
	writel(0x9c4000ff, &dmc1->phycontrol0);

	writel(0x5d, &dmc1->timingref);

	writel(0x34498691, &dmc1->timingrow);
	writel(0x36330306, &dmc1->timingdata);
	writel(0x50380365, &dmc1->timingpower);

	sdelay(0x100000);

	writel(0x07000000, &dmc1->directcmd);

	sdelay(0x100000);

	writel(0x00071c00, &dmc1->directcmd);

	sdelay(0x100000);

	writel(0x00010bfc, &dmc1->directcmd);

	sdelay(0x100000);

	writel(0x608, &dmc1->directcmd);
	writel(0x810, &dmc1->directcmd);
	writel(0xc08, &dmc1->directcmd);

	writel(PHYCONTROL0_VAL, &dmc->phycontrol0);
	writel(MEM_TERM_EN | PHY_READ_EN | CTRL_SHGATE | CTRL_REF(8) | CTRL_SHIFTC(4), &dmc->phycontrol1);
	writel(PHYCONTROL0_VAL | CTRL_DLL_START, &dmc->phycontrol0);
	sdelay(0x20000);

	writel(CTRL_REF(8) | CTRL_SHIFTC(4), &dmc->phycontrol1);
	writel(CTRL_REF(8) | FP_RESYNC | CTRL_SHIFTC(4), &dmc->phycontrol1);

	sdelay(0x20000);

	writel(PHYCONTROL0_VAL, &dmc1->phycontrol0);
	writel(MEM_TERM_EN | PHY_READ_EN | CTRL_SHGATE | CTRL_REF(8) | CTRL_SHIFTC(4), &dmc1->phycontrol1);
	writel(PHYCONTROL0_VAL | CTRL_DLL_START, &dmc1->phycontrol0);
	sdelay(0x20000);

	writel(CTRL_REF(8) | CTRL_SHIFTC(4), &dmc1->phycontrol1);
	writel(CTRL_REF(8) | FP_RESYNC | CTRL_SHIFTC(4), &dmc1->phycontrol1);

	sdelay(0x20000);

	writel(DMC_CONCONTROL, &dmc->concontrol);
	writel(DMC_CONCONTROL, &dmc1->concontrol);

	writel(DMC_MEMCONTROL, &dmc->memcontrol);
	writel(DMC_MEMCONTROL, &dmc1->memcontrol);

	tzasc_init();

}


