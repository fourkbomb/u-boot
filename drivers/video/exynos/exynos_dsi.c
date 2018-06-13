/*
 * Copyright (c) 2018 Simon Shields <simon@lineageos.org>
 * Copyright (c) 2012 Samsung Electronics
 * 
 * Author: InKi Dae <inki.dae@samsung.com>
 * Author: Donghwa Lee <dh09.lee@samsung.com>
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <common.h>
#include <clk.h>
#include <display.h>
#include <dm.h>
#include <fdtdec.h>
#include <panel.h>
#include <mipi_dsi.h>
#include <regmap.h>
#include <asm/arch/mipi_dsi.h>
#include "exynos_dsi.h"

#define MHZ (1000 * 1000)
#define FIN_HZ (24 * MHZ)

#define DFIN_PLL_MIN_HZ		(6 * MHZ)
#define DFIN_PLL_MAX_HZ		(12 * MHZ)

DECLARE_GLOBAL_DATA_PTR;

#define reg_read(offset) readl(priv->regs + offset)
#define reg_write(value, offset) writel(value, priv->regs + offset)

static unsigned int dpll_table[15] = {
	100, 120, 170, 220, 270,
	320, 390, 450, 510, 560,
	640, 690, 770, 870, 950
};

int exynos_mipi_read_timing(struct udevice *dev,
		struct display_timing *timing)
{
	int ret;

	ret = fdtdec_decode_display_timing(gd->fdt_blob, dev_of_offset(dev),
			0, timing);
	if (ret) {
		printf("%s: Failed to decode display timing (%d)\n",
				__func__, ret);
		return -EINVAL;
	}

	return 0;
}

static void exynos_mipi_clear_pll_stable(struct udevice *dev)
{
	struct exynos_mipi_priv *priv = dev_get_priv(dev);
	u32 reg = reg_read(DSI_INTSRC);
	reg |= DSI_INTSRC_PLL_STABLE;

	reg_write(reg, DSI_INTSRC);
}

static int exynos_mipi_pll_stable(struct udevice *dev)
{
	struct exynos_mipi_priv *priv = dev_get_priv(dev);
	u32 reg = reg_read(DSI_STATUS);

	return reg & DSI_STATUS_PLL_STABLE;
}

static void exynos_mipi_set_afc(struct udevice *dev, int en, int code)
{
	struct exynos_mipi_priv *priv = dev_get_priv(dev);

	u32 reg = reg_read(DSI_PHYACCHR);

	if (en) {
		reg |= DSI_AFC_EN;
		reg &= ~DSI_AFC_CTL_MASK;
		reg |= DSI_AFC_CTL(code);
	} else
		reg &= ~DSI_AFC_EN;

	reg_write(reg, DSI_PHYACCHR);
}

static long exynos_mipi_set_pll(struct udevice *dev)
{
	struct exynos_mipi_priv *priv = dev_get_priv(dev);
	unsigned long dfin_pll, fout;
	u32 reg;
	int afc_en, afc_code, freq_band, timeout, i;

	exynos_mipi_clear_pll_stable(dev);

	dfin_pll = (FIN_HZ / priv->p);

	if (dfin_pll < DFIN_PLL_MIN_HZ || dfin_pll > DFIN_PLL_MAX_HZ) {
		afc_en = 0;
		afc_code = 0;
	} else {
		afc_en = 1;
		if (dfin_pll < 7 * MHZ)
			afc_code = 1;
		else if (dfin_pll < 8 * MHZ)
			afc_code = 0;
		else if (dfin_pll < 9 * MHZ)
			afc_code = 3;
		else if (dfin_pll < 10 * MHZ)
			afc_code = 2;
		else if (dfin_pll < 11 * MHZ)
			afc_code = 5;
		else
			afc_code = 4;
	}
	printf("fin_pll: %lu. afc_en? %d, afc_code %d\n", dfin_pll, afc_en, afc_code);
	exynos_mipi_set_afc(dev, afc_en, afc_code);

	fout = (dfin_pll * priv->m) / (1 << priv->s);

	for (i = 0; i < ARRAY_SIZE(dpll_table); i++) {
		if (fout < dpll_table[i] * MHZ) {
			freq_band = i;
			printf("freq_band=%d\n", i);
			break;
		}
	}

	if (i == ARRAY_SIZE(dpll_table))
		return 0;

	reg_write(priv->pll_lock_time, DSI_PLLTMR);

	reg = reg_read(DSI_PLLCTRL);
	/* mask out whole register */
	reg &= ~DSI_PLLCTRL_MASK;
	reg |= DSI_PLLCTRL_P(priv->p) | DSI_PLLCTRL_M(priv->m) | DSI_PLLCTRL_S(priv->s);
	reg |= DSI_PLLCTRL_FREQBAND(freq_band) | DSI_PLLCTRL_EN;

	reg_write(reg, DSI_PLLCTRL);

	/* Wait for PLL to become stable */
	timeout = 1000;
	while (timeout > 0) {
		if (exynos_mipi_pll_stable(dev))
			return fout;
		timeout--;
	}

	printf("PLL not stable?\n");
	return 0;
}

int exynos_mipi_set_clock(struct udevice *dev)
{
	struct exynos_mipi_priv *priv = dev_get_priv(dev);
	u32 reg;
	int esc_div;
	u32 hs_clk, byte_clk, esc_clk;

	hs_clk = exynos_mipi_set_pll(dev);
	if (!hs_clk)
		return -EINVAL;
	byte_clk = hs_clk / 8;
	esc_div = byte_clk / priv->esc_clk;

	if ((byte_clk / esc_div) > (20 * MHZ) ||
			(byte_clk / esc_div) > priv->esc_clk)
		esc_div++;

	esc_clk = byte_clk / esc_div;

	reg = reg_read(DSI_CLKCTRL);
	reg |= DSI_CLKCTRL_BYTECLK_EN | DSI_CLKCTRL_ESCCLK_EN;
	reg |= esc_div & DSI_CLKCTRL_ESC_PRESCALER_MASK;
	reg |= DSI_CLKCTRL_LANE_ESCCLK((priv->lane_mask << 1) | 1);

	reg_write(reg, DSI_CLKCTRL);

	printf("%s: byte_clk=%u MHz, esc_clk=%uMHz, div=%d, goal=%uMHz\n", __func__, byte_clk, esc_clk, esc_div,
			priv->esc_clk);
	return 0;
}

void exynos_mipi_reset(struct udevice *dev)
{
	struct exynos_mipi_priv *priv = dev_get_priv(dev);
	u32 reg = reg_read(DSI_SWRST);

	reg |= DSI_SWRST_SWRST | DSI_SWRST_FUNCRST;

	reg_write(reg, DSI_SWRST);
}

int exynos_mipi_prepare(struct udevice *dev)
{
	struct exynos_mipi_priv *priv = dev_get_priv(dev);
	u32 reg;
	u32 mask = DSI_FIFOCTRL_MD_FIFO | DSI_FIFOCTRL_SD_FIFO
			   | DSI_FIFOCTRL_I80_FIFO | DSI_FIFOCTRL_SFR_FIFO
			   | DSI_FIFOCTRL_RX_FIFO;
	u32 timeout = 100;

	/* init FIFO */
	reg = reg_read(DSI_FIFOCTRL);
	reg &= ~mask;
	reg_write(reg, DSI_FIFOCTRL);
	printf("Udelay\n");
	udelay(10 * 1000);
	printf("Udelayed\n");

	reg |= mask;
	reg_write(reg, DSI_FIFOCTRL);

	/* init config sfr */
	reg = reg_read(DSI_CONFIG);
	reg &= ~(DSI_CONFIG_EOT_PACKET(1) | DSI_CONFIG_AUTO_MODE(1)
			| DSI_CONFIG_HSE_MODE(1) | DSI_CONFIG_HFP_MODE(1)
			| DSI_CONFIG_HBP_MODE(1) | DSI_CONFIG_HSA_MODE(1)
			| DSI_CONFIG_NUM_LANES(3));

	reg |= DSI_CONFIG_HFP_MODE(priv->hfp) | DSI_CONFIG_NUM_LANES(priv->num_lanes - 1)
		| DSI_CONFIG_LANE_ENABLE(priv->lane_mask) | DSI_CONFIG_CLOCK_LANE
		| DSI_CONFIG_AUTO_MODE(1);
	reg_write(reg, DSI_CONFIG);

	printf("Set mipi clock\n");
	exynos_mipi_set_clock(dev);
	printf("Set mipi clock.\n");

	/* Wait for the DSI master to enter the stop state */
	do {
		reg = reg_read(DSI_STATUS);
		if ((reg & DSI_STATUS_STOPSTATE(priv->lane_mask)) && (reg & DSI_STATUS_STOPSTATE_CLK))
			break;
		/* If something else has already initialised the DSI master, it might be in this state */
		if (reg & DSI_STATUS_TX_READY_HS_CLK)
			break;
		timeout--;
		if (!timeout) {
			printf("Failed to initialise DSI master (not in stop state!)\n");
			return -ENXIO;
		}
		printf(".");
	} while (timeout >= 0);

	printf("\nSTOP STATE: %#x\n", priv->stop_holding_cnt);
	reg = reg_read(DSI_ESCMODE);
	reg &= ~(DSI_ESCMODE_STOP_STATE_CNT(0x7ff));
	reg |= DSI_ESCMODE_STOP_STATE_CNT(priv->stop_holding_cnt);
	printf("reg: %#x\n", reg);
	reg_write(reg, DSI_ESCMODE);

	reg = reg_read(DSI_TIMEOUT);
	reg &= ~(DSI_TIMEOUT_LPDR(0xffff) | DSI_TIMEOUT_BTA(0xff));
	reg |= DSI_TIMEOUT_LPDR(priv->rx_timeout) | DSI_TIMEOUT_BTA(priv->bta_timeout);
	reg_write(reg, DSI_TIMEOUT);
	return 0;
}

void exynos_mipi_hs_enable(struct udevice *dev)
{
	struct exynos_mipi_priv *priv = dev_get_priv(dev);
	u32 reg = reg_read(DSI_ESCMODE);

	reg &= DSI_ESCMODE_TX_LPDT | DSI_ESCMODE_CMD_LPDT;
	reg_write(reg, DSI_ESCMODE);

	reg = reg_read(DSI_CLKCTRL);
	reg |= DSI_CLKCTRL_HSCLK_EN;
	reg_write(reg, DSI_CLKCTRL);
}

void exynos_mipi_set_display_mode(struct udevice *dev,
		const struct display_timing *timing)
{
	struct exynos_mipi_priv *priv = dev_get_priv(dev);
	u32 reg;

	/* vporch */
	reg = reg_read(DSI_MVPORCH);
	reg &= ~(DSI_MVPORCH_CMD_ALLOW(0xf)
			| DSI_MVPORCH_STABLE_VFP(0x7ff)
			| DSI_MVPORCH_MAIN_VBP(0x7ff));
	reg |= DSI_MVPORCH_CMD_ALLOW(0xf) /* Linux hardcodes this as well */
		| DSI_MVPORCH_STABLE_VFP(timing->vfront_porch.typ)
		| DSI_MVPORCH_MAIN_VBP(timing->vback_porch.typ);
	reg_write(reg, DSI_MVPORCH);

	/* hporch */
	reg = reg_read(DSI_MHPORCH);
	reg &= ~(DSI_MHPORCH_MAIN_HFP(0xffff)
			| DSI_MHPORCH_MAIN_HBP(0xffff));
	reg |= DSI_MHPORCH_MAIN_HFP(timing->hfront_porch.typ)
		| DSI_MHPORCH_MAIN_HBP(timing->hback_porch.typ);
	reg_write(reg, DSI_MHPORCH);

	/* hsync/vsync */
	reg = reg_read(DSI_MSYNC);
	reg &= ~(DSI_MSYNC_VSYNC(0x3ff) | DSI_MSYNC_HSYNC(0xffff));
	reg |= DSI_MSYNC_VSYNC(timing->vsync_len.typ)
		| DSI_MSYNC_HSYNC(timing->hsync_len.typ);
	reg_write(reg, DSI_MSYNC);

	/* resolution */
	reg = reg_read(DSI_MDRESOL);
	reg &= ~DSI_MDRESOL_STANDBY;
	reg |= DSI_MDRESOL_VERTICAL(timing->vactive.typ)
		| DSI_MDRESOL_HORIZONTAL(timing->hactive.typ);
	reg_write(reg, DSI_MDRESOL);

	/* display config */
	reg = reg_read(DSI_CONFIG);
	reg &= ~(DSI_CONFIG_BURST_MODE(3) | DSI_CONFIG_MAINVC(0x3)
			| DSI_CONFIG_MAINPIX(0x7));

	/* if (!command_mode) { */
	reg |= DSI_CONFIG_VIDEO_MODE(1);
	/* } */
	reg |= DSI_CONFIG_BURST_MODE(priv->burst_mode) |
		DSI_CONFIG_MAINVC(priv->virtual_channel) |
		DSI_CONFIG_MAINPIX(priv->pixel_format);
	reg_write(reg, DSI_CONFIG);
}

static void exynos_mipi_dsi_write_header(struct udevice *dev, unsigned int type,
		unsigned char d0, unsigned char d1)
{
	struct exynos_mipi_priv *priv = dev_get_priv(dev);
	unsigned int header = DSI_PKTHDR_DAT1(d1) | DSI_PKTHDR_DAT0(d0) | DSI_PKTHDR_DI(type);
	printf("Write header: type=%d, d0=%#x, d1=%#x, header=%#x\n", type, d0, d1, header);
	reg_write(header, DSI_PKTHDR);
}

static void exynos_mipi_dsi_long_write(struct udevice *dev, unsigned char *data,
		size_t size)
{
	struct exynos_mipi_priv *priv = dev_get_priv(dev);
	unsigned int payload;
	int extra = size % 4;
	int aligned = size - extra;

	printf("Writing %d bytes (%d + %d)\n", size, aligned, extra);
	/* write 4 bytes at a time */
	for (int i = 0; i < aligned; i += 4) {
		payload = data[i] |
			(data[i + 1] << 8) |
			(data[i + 2] << 16) |
			(data[i + 3] << 24);

		printf("%#x ", payload);
		reg_write(payload, DSI_PAYLOAD);
	}
	printf("\n");

	if (extra) {
		/* Write any remaining data */
		payload = data[aligned];
		if (extra >= 2)
			payload |= data[aligned + 1] << 8;
		if (extra >= 3)
			payload |= data[aligned + 2] << 16;

		printf("Extra data: %#x\n", payload);
		reg_write(payload, DSI_PAYLOAD);
	}
}

int exynos_mipi_dsi_write(struct udevice *dev, enum dsi_cmd_type type,
		unsigned char *data, size_t size)
{
	struct exynos_mipi_priv *priv = dev_get_priv(dev);
	unsigned long delay = 10 * MHZ / priv->esc_clk;

	mdelay(delay);

	printf("Write seq (type=%d) starting with %#x, len=%d\n",type, data[0], size);

	switch (type) {
	case MIPI_DSI_DCS_SHORT_WRITE:
	case MIPI_DSI_DCS_SHORT_WRITE_PARAM:
		if (size < 2)
			return -EINVAL;
		exynos_mipi_dsi_write_header(dev, type, data[0], data[1]);
		break;
	case MIPI_DSI_DCS_LONG_WRITE:
		exynos_mipi_dsi_long_write(dev, data, size);
		exynos_mipi_dsi_write_header(dev, type, size & 0xff, (size & 0xff00) >> 8);
		break;
	default:
		printf("%s: type %d not supported\n", __func__, type);
		return -EOPNOTSUPP;
	}

	return 0;
}
