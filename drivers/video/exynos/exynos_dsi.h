/*
 * Copyright (c) 2018 Simon Shields <simon@lineageos.org>
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#ifndef __EXYNOS_DSI_H
#define __EXYNOS_DSI_H

struct exynos_mipi_priv {
	uintptr_t regs;

	int lane_mask;
	u32 num_lanes;
	u32 hfp;
	u32 stop_holding_cnt;
	u32 bta_timeout;
	u32 rx_timeout;

	u32 virtual_channel;
	u32 burst_mode;
	u32 pixel_format;

	/* PLL config */
	u32 p;
	u32 m;
	u32 s;
	u32 pll_lock_time;

	// ...
	int e_interface;
	int e_byte_clk;

	u32 pll_stable_time;
	u32 esc_clk;
};

int exynos_mipi_read_timing(struct udevice *dev,
		struct display_timing *timing);

int exynos_mipi_set_clock(struct udevice *dev);
void exynos_mipi_reset(struct udevice *dev);
int exynos_mipi_prepare(struct udevice *dev);
void exynos_mipi_hs_enable(struct udevice *dev);
void exynos_mipi_set_display_mode(struct udevice *dev,
		const struct display_timing *timing);
int exynos_mipi_dsi_write(struct udevice *dev, enum dsi_cmd_type type,
		unsigned char *data, size_t size);

#endif
