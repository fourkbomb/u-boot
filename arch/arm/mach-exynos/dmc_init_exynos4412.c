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

static int board_num_mem_chips(void)
{
	u32 pkgid = readl(EXYNOS4_PRO_ID + 4);
	/* 2GB of RAM */
	if ((pkgid & 0x30) == 0x10)
		return 2;
	return 1;
}

static void set_prime_stopctrl(void)
{
	struct exynos4x12_clock *clk = (struct exynos4x12_clock *)samsung_get_base_clock();

	/* PRE_WAIT_CNT, POST_WAIT_CNT = 0x1 */
	writel(0x101, &clk->atclk_stopctrl);
}

static void do_directcmd(struct exynos4_dmc *dmc, u32 cmd, int chip, u32 delay)
{
	if (delay)
		sdelay(delay);
	if (chip)
		cmd |= CMD_CHIP(1);

	writel(cmd, &dmc->directcmd);
}

void mem_ctrl_init(int reset)
{
	struct exynos4_dmc *dmcs[2];
	dmcs[0] = (struct exynos4_dmc *)samsung_get_base_dmc_ctrl();
	dmcs[1] = (struct exynos4_dmc *)(samsung_get_base_dmc_ctrl() + DMC_OFFSET);
	int chips = board_num_mem_chips();
	int rev = exynos4412_get_rev();
	u32 memcontrol = DMC_MEMCONTROL;
	u32 prechconfig = 0x64000000;
	u32 zqcontrol = DMC_PHYZQCONTROL;

	if (rev == EXYNOS4412_REV_ZERO)
		prechconfig |= 0xffff;
	else if (rev == EXYNOS4412_REV_PRIME)
		set_prime_stopctrl();

	if (chips == 2) {
		memcontrol |= MEM_2CHIPS;
		zqcontrol |= CTRL_ZQ_MODE_DDS_2GB;
	} else {
		zqcontrol |= CTRL_ZQ_MODE_DDS_1GB;
	}

	for (int idx = 0; idx < ARRAY_SIZE(dmcs); idx++) {
		struct exynos4_dmc *dmc = dmcs[idx];
		writel(zqcontrol, &dmc->phyzqcontrol);

		writel(PHYCONTROL0_VAL_INIT, &dmc->phycontrol0);
		writel(PHYCONTROL0_VAL_INIT | CTRL_DLL_ON, &dmc->phycontrol0);

		writel(PHYCONTROL1_VAL, &dmc->phycontrol1);
		writel(PHYCONTROL0_VAL, &dmc->phycontrol0);
		writel(PHYCONTROL1_VAL | FP_RESYNC, &dmc->phycontrol1);
		writel(PHYCONTROL1_VAL, &dmc->phycontrol1);
		writel(PHYCONTROL1_VAL | FP_RESYNC, &dmc->phycontrol1);
		writel(PHYCONTROL1_VAL, &dmc->phycontrol1);

		writel(DMC_CONCONTROL_INIT, &dmc->concontrol);
		writel(memcontrol, &dmc->memcontrol);
		writel(CHIP_BASE(0x40) | DMC_MEMCONFIG0, &dmc->memconfig0);
		if (chips == 2)
			writel(CHIP_BASE(0x80) | DMC_MEMCONFIG0, &dmc->memconfig1);
		writel(DMC_IVCONTROL, &dmc->ivcontrol);
		writel(prechconfig, &dmc->prechconfig);
		writel(PHYCONTROL0_VAL_STAGE2, &dmc->phycontrol0);

		writel(T_REFI(0x5d), &dmc->timingref);

		if (rev == EXYNOS4412_REV_PRIME) {
			writel(T_RFC(0x3a) | T_RRD(0x5) | T_RP(0xa) | T_RCD(0x8) |
					T_RC(0x1c) | T_RAS(0x13), &dmc->timingrow);
			writel(T_WTR(0x4) | T_WR(0x7) | T_RTP(0x4) | CL(0x0) |
					WL(0x3) | RL(0x6), &dmc->timingdata);
			writel(T_FAW(0x16) | T_XSR(0x3e) | T_XP(0x4) | T_CKE(0x7) | T_MRD(0x5),
					&dmc->timingpower);
		} else {
			u32 timingrow = T_RFC(0x34) | T_RP(0x9) | T_RCD(0x8) |
				T_RC(0x1a) | T_RAS(0x11);
			if (rev == EXYNOS4412_REV_ZERO)
				timingrow |= T_RRD(0xa);
			else
				timingrow |= T_RRD(0x4);
			writel(timingrow, &dmc->timingrow);
			writel(T_WTR(0x3) | T_WR(0x6) | T_RTP(0x3) | CL(0x3) |
					WL(0x3) | RL(0x6), &dmc->timingdata);
			writel(T_FAW(0x14) | T_XSR(0x38) | T_XP(0x3) | T_CKE(0x6) |
					T_MRD(0x5), &dmc->timingpower);
		}

		for (int i = 0; i < chips; i++) {
			// TODO: reduce delay values
			do_directcmd(dmc, CMD_TYPE(0x7), i, 0x100000);
			do_directcmd(dmc, CMD_TYPE(0x0) | CMD_BANK(0x7) | CMD_ADDR(0x1c00), i, 0x100000);
			do_directcmd(dmc, CMD_TYPE(0x0) | CMD_BANK(0x1) | CMD_ADDR(0xbfc), i, 0x100000);
			do_directcmd(dmc, CMD_TYPE(0x0) | CMD_BANK(0x0) | CMD_ADDR(0x608), i, 0x100000);
			do_directcmd(dmc, CMD_TYPE(0x0) | CMD_BANK(0x0) | CMD_ADDR(0x810), i, 0);
			do_directcmd(dmc, CMD_TYPE(0x0) | CMD_BANK(0x0) | CMD_ADDR(0xc08), i, 0);
		}
	}

	writel(PHYCONTROL0_VAL, &dmcs[0]->phycontrol0);
	writel(MEM_TERM_EN | PHY_READ_EN | CTRL_SHGATE | CTRL_REF(8) | CTRL_SHIFTC(4), &dmcs[0]->phycontrol1);
	writel(PHYCONTROL0_VAL | CTRL_DLL_START, &dmcs[0]->phycontrol0);
	sdelay(0x20000);

	writel(CTRL_REF(8) | FP_RESYNC | CTRL_SHIFTC(4), &dmcs[0]->phycontrol1);
	writel(CTRL_REF(8) | CTRL_SHIFTC(4), &dmcs[0]->phycontrol1);

	sdelay(0x20000);

	writel(PHYCONTROL0_VAL, &dmcs[1]->phycontrol0);
	writel(MEM_TERM_EN | PHY_READ_EN | CTRL_SHGATE | CTRL_REF(8) | CTRL_SHIFTC(4), &dmcs[1]->phycontrol1);
	writel(PHYCONTROL0_VAL | CTRL_DLL_START, &dmcs[1]->phycontrol0);
	sdelay(0x20000);

	writel(CTRL_REF(8) | FP_RESYNC | CTRL_SHIFTC(4), &dmcs[1]->phycontrol1);
	writel(CTRL_REF(8) | CTRL_SHIFTC(4), &dmcs[1]->phycontrol1);

	sdelay(0x20000);

	writel(DMC_CONCONTROL, &dmcs[0]->concontrol);
	writel(DMC_CONCONTROL, &dmcs[1]->concontrol);

	memcontrol |= DSREF_EN | TP_EN | DPWRDN_EN | CLK_STOP_EN;
	writel(memcontrol, &dmcs[0]->memcontrol);
	writel(memcontrol, &dmcs[1]->memcontrol);

	tzasc_init();

}


