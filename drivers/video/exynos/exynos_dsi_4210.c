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
#include <regmap.h>
#include <asm/arch/mipi_dsi.h>
#include "exynos_dsi.h"

DECLARE_GLOBAL_DATA_PTR;

static int exynos_mipi_enable(struct udevice *dev, int panel_bpp,
		const struct display_timing *timing)
{
	int ret;

	printf("Reset\n");
	exynos_mipi_reset(dev);
	printf("Prepare\n");
	ret = exynos_mipi_prepare(dev);
	if (ret) {
		printf("%s: mipi_prepare failed: %d\n", __func__, ret);
		return ret;
	}

	printf("HS enable\n");
	exynos_mipi_hs_enable(dev);
	printf("Set display mode\n");
	exynos_mipi_set_display_mode(dev, timing);
	printf("All g\n");

	return 0;
}

static int exynos_mipi_ofdata_to_platdata(struct udevice *dev)
{
	struct exynos_mipi_priv *priv = dev_get_priv(dev);
	int ret;

	priv->regs = dev_read_addr(dev);
	if (priv->regs == FDT_ADDR_T_NONE) {
		printf("%s: found no MIPI dsi address (%lu)\n", __func__,
				priv->regs);
		return -ENXIO;
	}

	ret = dev_read_u32(dev, "samsung,data-lanes", &priv->num_lanes);
	if (ret) {
		printf("%s: Failed to get number of lanes: %d\n", __func__,
				ret);
		return ret;
	}

	ret = dev_read_u32(dev, "samsung,stop-holding-cnt", &priv->stop_holding_cnt);
	if (ret) {
		printf("%s: Failed to get stop-holding-cnt: %d\n", __func__, ret);
		return ret;
	}

	ret = dev_read_u32(dev, "samsung,bta-timeout", &priv->bta_timeout);
	if (ret) {
		printf("%s: Failed to get bta-timeout: %d\n", __func__, ret);
		return ret;
	}

	ret = dev_read_u32(dev, "samsung,rx-timeout", &priv->rx_timeout);
	if (ret) {
		printf("%s: Failed to get rx-timeout: %d\n", __func__, ret);
		return ret;
	}

	ret = dev_read_u32(dev, "samsung,burst-mode", &priv->burst_mode);
	if (ret) {
		printf("%s: Failed to get burst-mode: %d\n", __func__, ret);
		return ret;
	}

	ret = dev_read_u32(dev, "samsung,virtual-channel", &priv->virtual_channel);
	if (ret) {
		printf("%s: Failed to get virtual channel: %d\n", __func__, ret);
		return ret;
	}

	ret = dev_read_u32(dev, "samsung,pixel-format", &priv->pixel_format);
	if (ret) {
		printf("%s: Failed to get pixel format: %d\n", __func__, ret);
		return ret;
	}

	ret = dev_read_u32(dev, "samsung,esc-clk", &priv->esc_clk);
	if (ret) {
		printf("%s: Failed to get esc clk: %d\n", __func__, ret);
		return ret;
	}

	ret = dev_read_u32(dev, "samsung,pll-pre-divider", &priv->p);
	ret = dev_read_u32(dev, "samsung,pll-main-divider", &priv->m);
	ret = dev_read_u32(dev, "samsung,pll-scaler", &priv->s);

	priv->lane_mask = 0;
	for (int i = 0; i < priv->num_lanes; i++)
		priv->lane_mask |= (1 << i);

	return 0;
}

static int exynos_mipi_probe(struct udevice *dev)
{
#if 0
	int ret;
	struct exynos_mipi_priv *priv = dev_get_priv(dev);

	ret = uclass_get_device_by_phandle(UCLASS_PANEL, dev, "samsung,panel",
			&priv->panel);
	if (ret) {
		printf("%s: Can't find panel (err=%d)\n", __func__, ret);
		return ret;
	}

#endif
	printf("%s: Probed mipi DSI driver\n", __func__);
	return 0;
}

static const struct dm_display_ops exynos_mipi_dsi_ops = {
	.read_timing = exynos_mipi_read_timing,
	.enable = exynos_mipi_enable,
};

static const struct udevice_id exynos_mipi_dsi_ids[] = {
	{ .compatible = "samsung,exynos4210-mipi-dsi" },
	{ }
};

U_BOOT_DRIVER(exynos_mipi_dsi) = {
	.name = "exynos_mipi_dsi",
	.id = UCLASS_DISPLAY,
	.of_match = exynos_mipi_dsi_ids,
	.ofdata_to_platdata = exynos_mipi_ofdata_to_platdata,
	.probe = exynos_mipi_probe,
	.ops = &exynos_mipi_dsi_ops,
	.priv_auto_alloc_size = sizeof(struct exynos_mipi_priv),
};
