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

// TODO: 4412 and 4210 should be commonised using something like is_soc_4210
void system_clock_init(void)
{
	struct exynos4x12_clock *clk =
		(struct exynos4x12_clock *)samsung_get_base_clock();
	struct exynos4412_power *power = (struct exynos4412_power *)samsung_get_base_power();
	struct exynos4_dmc *dmc = (struct exynos4_dmc *)samsung_get_base_dmc_ctrl();
	struct exynos4_dmc *dmc1 = (struct exynos4_dmc *)(samsung_get_base_dmc_ctrl() + DMC_OFFSET);

	// disable WDT
	writel(0, (unsigned int *)0x10060000);

	writel(0, &clk->src_cpu);

	sdelay(0x10000);

	writel(CLK_DIV_DMC0_VAL, &clk->div_dmc0);
	writel(CLK_DIV_DMC1_VAL, &clk->div_dmc1);

	writel(CLK_SRC_TOP0_VAL, &clk->src_top0);
	writel(CLK_SRC_TOP1_VAL, &clk->src_top1);
	sdelay(0x10000);
	writel(CLK_DIV_TOP_VAL, &clk->div_top);

	writel(CLK_SRC_LEFTBUS_VAL, &clk->src_leftbus);
	sdelay(0x10000);
	writel(CLK_DIV_LEFTBUS_VAL, &clk->div_leftbus);

	writel(CLK_SRC_RIGHTBUS_VAL, &clk->src_rightbus);
	sdelay(0x10000);
	writel(CLK_DIV_RIGHTBUS_VAL, &clk->div_rightbus);

	// setup PLLs
	writel(0x3e8, &clk->apll_lock);
	writel(0x2f1, &clk->mpll_lock);
	writel(0x2321, &clk->epll_lock);
	writel(0x2321, &clk->vpll_lock);

	writel(CLK_DIV_CPU0_VAL, &clk->div_cpu0);
	writel(CLK_DIV_CPU1_VAL, &clk->div_cpu1);

	writel(APLL_CON1_VAL, &clk->apll_con1);
	writel(APLL_CON0_VAL, &clk->apll_con0);

	/* don't reset MPLL in C2C case */
	if (readl(&clk->mpll_con1) == 0xa0640301) {
		writel(MPLL_CON1_VAL, &clk->mpll_con1);
		writel(MPLL_CON0_VAL, &clk->mpll_con0);
	}

	writel(EPLL_CON2_VAL, &clk->epll_con2);
	writel(EPLL_CON1_VAL, &clk->epll_con1);
	writel(EPLL_CON0_VAL, &clk->epll_con0);

	writel(VPLL_CON2_VAL, &clk->vpll_con2);
	writel(VPLL_CON1_VAL, &clk->vpll_con1);
	writel(VPLL_CON0_VAL, &clk->vpll_con0);

	sdelay(0x10000);

	/* wtf */
	writel(0x01000001, &clk->src_cpu);
	writel(0x00011000, &clk->src_dmc);
	writel(0x00000110, &clk->src_top0);
	writel(0x01111000, &clk->src_top1);

	sdelay(0x10000);

	if ((readl(&power->c2c_ctrl) & 1) != 0) {
		return;
	}

	/* todo: split this out, clean it up. Or drop it entirely? */
	// DRAM clocks
	writel(0x7f10100a, &dmc->phycontrol0);
	writel(0xe0000084, &dmc->phycontrol1);
	writel(0x7f10100b, &dmc->phycontrol0);

	sdelay(0x20000);

	writel(0x0000008c, &dmc->phycontrol1); // force DLL resync
	writel(0x00000084, &dmc->phycontrol1);

	sdelay(0x20000);

	writel(0x0fff30fa, &dmc->concontrol);
	writel(0x0fff30fa, &dmc1->concontrol);

	writel(0x00202533, &dmc->memcontrol);
	writel(0x00202533, &dmc1->memcontrol);
}

void mem_ctrl_init(int reset)
{
	struct exynos4x12_clock *clk =
		(struct exynos4x12_clock *)samsung_get_base_clock();
	struct exynos4_dmc *dmc = (struct exynos4_dmc *)samsung_get_base_dmc_ctrl();

	writel(0x00117713, &clk->div_dmc0);

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

	dmc = (struct exynos4_dmc *)(samsung_get_base_dmc_ctrl() + DMC_OFFSET);

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
}


