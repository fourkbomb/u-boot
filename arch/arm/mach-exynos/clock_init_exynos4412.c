/*
 * Clock initialisation for Exynos4412 based boards
 *
 * Copyright (C) 2018 Simon Shields <simon@lineageos.org>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <config.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/clk.h>
#include <asm/arch/clock.h>
#include <asm/arch/power.h>
#include "common_setup.h"
#include "exynos4412_setup.h"

static void reset_isp(void)
{
	struct exynos4412_power *pwr =
		(struct exynos4412_power *)samsung_get_base_power();

	writel(0, &pwr->cmu_reset_isp_sys_pwr_reg);
	writel(0, &pwr->cmu_sysclk_isp_sys_pwr_reg);
}

void system_clock_init(void)
{
	struct exynos4x12_clock *clk =
		(struct exynos4x12_clock *)samsung_get_base_clock();

	reset_isp();

	/* Switch clocks away from PLLs while we configure them */
	writel(CLK_SRC_CPU_INIT, &clk->src_cpu);
	writel(CLK_SRC_TOP0_INIT, &clk->src_top0);
	writel(CLK_SRC_TOP1_INIT, &clk->src_top1);
	writel(CLK_SRC_LEFTBUS_VAL, &clk->src_leftbus);
	writel(CLK_SRC_RIGHTBUS_VAL, &clk->src_rightbus);
	writel(CLK_SRC_PERIL0_VAL, &clk->src_peril0);
	writel(CLK_SRC_LCD0_VAL, &clk->src_lcd);

	sdelay(0x10000);

	writel(CLK_DIV_DMC0_VAL, &clk->div_dmc0);
	writel(CLK_DIV_DMC1_VAL, &clk->div_dmc1);
	writel(CLK_DIV_TOP_VAL, &clk->div_top);
	writel(CLK_DIV_LEFTBUS_VAL, &clk->div_leftbus);
	writel(CLK_DIV_RIGHTBUS_VAL, &clk->div_rightbus);
	writel(CLK_DIV_PERIL0_VAL, &clk->div_peril0);

	/* PLLs */
	writel(APLL_LOCK_VAL, &clk->apll_lock);
	writel(MPLL_LOCK_VAL, &clk->mpll_lock);
	writel(EPLL_LOCK_VAL, &clk->epll_lock);
	writel(VPLL_LOCK_VAL, &clk->vpll_lock);

	writel(CLK_DIV_CPU0_VAL, &clk->div_cpu0);
	writel(CLK_DIV_CPU1_VAL, &clk->div_cpu1);

	/* APLL: 800MHz */
	writel(APLL_CON1_VAL, &clk->apll_con1);
	writel(APLL_CON0_VAL, &clk->apll_con0);

	/* The iROM sets MPLL at 400MHz.
	 * Skip increasing MPLL if it's not at 400MHz */
	if (readl(&clk->mpll_con0) == 0xa0640301) {
		/* MPLL: 800MHz */
		writel(MPLL_CON1_VAL, &clk->mpll_con1);
		writel(MPLL_CON0_VAL, &clk->mpll_con0);
	}

	/* EPLL: 96MHz */
	/* (64) * (24000000) / (2 * (1 << 3)) */
	writel(EPLL_CON2_VAL, &clk->epll_con2);
	writel(EPLL_CON1_VAL, &clk->epll_con1);
	writel(EPLL_CON0_VAL, &clk->epll_con0);

	/* VPLL: 108MHz */
	/* (72) * (24000000) / (2 * (1 << 3)) */
	writel(VPLL_CON2_VAL, &clk->vpll_con2);
	writel(VPLL_CON1_VAL, &clk->vpll_con1);
	writel(VPLL_CON0_VAL, &clk->vpll_con0);

	/* Stabilise */
	sdelay(0x40000);

	/* Now that PLLs are set up, we can set the other clocks to use them... */
	writel(CLK_SRC_CPU_PLLS, &clk->src_cpu);
	writel(CLK_SRC_DMC_PLLS, &clk->src_dmc);
	writel(CLK_SRC_TOP0_PLLS, &clk->src_top0);
	writel(CLK_SRC_TOP1_PLLS, &clk->src_top1);

	sdelay(0x10000);

	/*
	 * In the SDMMC booting case, we need to reconfigure MMC clock
	 * to make the iROM happy.
	 */
	u32 fsys2_div = readl(&clk->div_fsys2);
	/* new MMC2 div is 16 */
	fsys2_div |= 0xf;
	writel(fsys2_div, &clk->div_fsys2);
}

void emmc_boot_clk_div_set(void)
{
	struct exynos4x12_clock *clk = (struct exynos4x12_clock *)samsung_get_base_clock();
	u32 div_fsys3 = readl(&clk->div_fsys3);

	div_fsys3 &= ~(0xff0f);
	div_fsys3 |= (1 << 8) | 0x7;

	writel(div_fsys3, &clk->div_fsys3);
}
