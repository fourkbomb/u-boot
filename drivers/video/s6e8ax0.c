/*
 * Copyright (C) 2012 Samsung Electronics
 *
 * Author: Donghwa Lee <dh09.lee@samsung.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <display.h>
#include <dm.h>
#include <asm/gpio.h>
#include <mipi_dsi.h>
#include <panel.h>
#include <power/regulator.h>

struct s6e8ax0_priv {
	struct udevice *vci_reg;
	struct udevice *vdd3_reg;
	struct udevice *display;
	struct gpio_desc reset_gpio;
};

static void s6e8ax0_panel_cond(struct s6e8ax0_priv *priv)
{
	struct udevice *display = priv->display;
	int reverse = 1;
	static unsigned char data_to_send[] = {
		0xf8, 0x3d, 0x35, 0x00, 0x00, 0x00, 0x8d, 0x00, 0x4c,
		0x6e, 0x10, 0x27, 0x7d, 0x3f, 0x10, 0x00, 0x00, 0x20,
		0x04, 0x08, 0x6e, 0x00, 0x00, 0x00, 0x02, 0x08, 0x08,
		0x23, 0x23, 0xc0, 0xc8, 0x08, 0x48, 0xc1, 0x00, 0xc3,
		0xff, 0xff, 0xc8
	};

	static unsigned char data_to_send_reverse[] = {
		0xf8, 0x19, 0x35, 0x00, 0x00, 0x00, 0x93, 0x00, 0x3c,
		0x7d, 0x08, 0x27, 0x7d, 0x3f, 0x00, 0x00, 0x00, 0x20,
		0x04, 0x08, 0x6e, 0x00, 0x00, 0x00, 0x02, 0x08, 0x08,
		0x23, 0x23, 0xc0, 0xc1, 0x01, 0x41, 0xc1, 0x00, 0xc1,
		0xf6, 0xf6, 0xc1
	};

	if (reverse) {
		display_write(display, MIPI_DSI_DCS_LONG_WRITE,
			data_to_send_reverse,
			ARRAY_SIZE(data_to_send_reverse));
	} else {
		display_write(display, MIPI_DSI_DCS_LONG_WRITE,
			data_to_send, ARRAY_SIZE(data_to_send));
	}
}

static void s6e8ax0_display_cond(struct s6e8ax0_priv *priv)
{
	struct udevice *display = priv->display;
	static unsigned char data_to_send[] = {
		0xf2, 0x80, 0x03, 0x0d
	};

	display_write(display, MIPI_DSI_DCS_LONG_WRITE,
			data_to_send, ARRAY_SIZE(data_to_send));
}

static void s6e8ax0_gamma_cond(struct s6e8ax0_priv *priv)
{
	struct udevice *display = priv->display;
	/* 7500K 2.2 Set : 30cd */
	static unsigned char data_to_send[] = {
		0xfa, 0x01, 0x60, 0x10, 0x60, 0xf5, 0x00, 0xff, 0xad,
		0xaf, 0xba, 0xc3, 0xd8, 0xc5, 0x9f, 0xc6, 0x9e, 0xc1,
		0xdc, 0xc0, 0x00, 0x61, 0x00, 0x5a, 0x00, 0x74,
	};

	display_write(display, MIPI_DSI_DCS_LONG_WRITE,
			data_to_send, ARRAY_SIZE(data_to_send));
}

static void s6e8ax0_gamma_update(struct s6e8ax0_priv *priv)
{
	struct udevice *display = priv->display;
	static unsigned char data_to_send[] = {
		0xf7, 0x03
	};

	display_write(display, MIPI_DSI_DCS_SHORT_WRITE_PARAM, data_to_send,
			ARRAY_SIZE(data_to_send));
}

static void s6e8ax0_etc_source_control(struct s6e8ax0_priv *priv)
{
	struct udevice *display = priv->display;
	static unsigned char data_to_send[] = {
		0xf6, 0x00, 0x02, 0x00
	};

	display_write(display, MIPI_DSI_DCS_LONG_WRITE,
			data_to_send, ARRAY_SIZE(data_to_send));
}

static void s6e8ax0_etc_pentile_control(struct s6e8ax0_priv *priv)
{
	struct udevice *display = priv->display;
	static unsigned char data_to_send[] = {
		0xb6, 0x0c, 0x02, 0x03, 0x32, 0xff, 0x44, 0x44, 0xc0,
		0x00
	};

	display_write(display, MIPI_DSI_DCS_LONG_WRITE,
			data_to_send, ARRAY_SIZE(data_to_send));
}

static void s6e8ax0_etc_mipi_control1(struct s6e8ax0_priv *priv)
{
	struct udevice *display = priv->display;
	static unsigned char data_to_send[] = {
		0xe1, 0x10, 0x1c, 0x17, 0x08, 0x1d
	};

	display_write(display, MIPI_DSI_DCS_LONG_WRITE,
			data_to_send, ARRAY_SIZE(data_to_send));
}

static void s6e8ax0_etc_mipi_control2(struct s6e8ax0_priv *priv)
{
	struct udevice *display = priv->display;
	static unsigned char data_to_send[] = {
		0xe2, 0xed, 0x07, 0xc3, 0x13, 0x0d, 0x03
	};

	display_write(display, MIPI_DSI_DCS_LONG_WRITE,
			data_to_send, ARRAY_SIZE(data_to_send));
}

static void s6e8ax0_etc_power_control(struct s6e8ax0_priv *priv)
{
	struct udevice *display = priv->display;
	static unsigned char data_to_send[] = {
		0xf4, 0xcf, 0x0a, 0x12, 0x10, 0x19, 0x33, 0x02
	};

	display_write(display, MIPI_DSI_DCS_LONG_WRITE,
		data_to_send, ARRAY_SIZE(data_to_send));
}

static void s6e8ax0_etc_mipi_control3(struct s6e8ax0_priv *priv)
{
	struct udevice *display = priv->display;
	static unsigned char data_to_send[] = {
		0xe3, 0x40
	};

	display_write(display, MIPI_DSI_DCS_SHORT_WRITE_PARAM, data_to_send,
		       ARRAY_SIZE(data_to_send));
}

static void s6e8ax0_etc_mipi_control4(struct s6e8ax0_priv *priv)
{
	struct udevice *display = priv->display;
	static unsigned char data_to_send[] = {
		0xe4, 0x00, 0x00, 0x14, 0x80, 0x00, 0x00, 0x00
	};

	display_write(display, MIPI_DSI_DCS_LONG_WRITE,
		data_to_send, ARRAY_SIZE(data_to_send));
}

static void s6e8ax0_elvss_set(struct s6e8ax0_priv *priv)
{
	struct udevice *display = priv->display;
	static unsigned char data_to_send[] = {
		0xb1, 0x04, 0x00
	};

	display_write(display, MIPI_DSI_DCS_LONG_WRITE,
			data_to_send, ARRAY_SIZE(data_to_send));
}

static void s6e8ax0_display_on(struct s6e8ax0_priv *priv)
{
	struct udevice *display = priv->display;
	static unsigned char data_to_send[] = {
		0x29, 0x00
	};

	display_write(display, MIPI_DSI_DCS_SHORT_WRITE, data_to_send,
		       ARRAY_SIZE(data_to_send));
}

static void s6e8ax0_sleep_out(struct s6e8ax0_priv *priv)
{
	struct udevice *display = priv->display;
	static unsigned char data_to_send[] = {
		0x11, 0x00
	};

	display_write(display, MIPI_DSI_DCS_SHORT_WRITE, data_to_send,
		       ARRAY_SIZE(data_to_send));
}

static void s6e8ax0_apply_level1_key(struct s6e8ax0_priv *priv)
{
	struct udevice *display = priv->display;
	static unsigned char data_to_send[] = {
		0xf0, 0x5a, 0x5a
	};

	display_write(display, MIPI_DSI_DCS_LONG_WRITE,
		data_to_send, ARRAY_SIZE(data_to_send));
}

static void s6e8ax0_apply_mtp_key(struct s6e8ax0_priv *priv)
{
	struct udevice *display = priv->display;
	static unsigned char data_to_send[] = {
		0xf1, 0x5a, 0x5a
	};

	display_write(display, MIPI_DSI_DCS_LONG_WRITE,
		data_to_send, ARRAY_SIZE(data_to_send));
}

static void s6e8ax0_panel_init(struct s6e8ax0_priv *priv)
{
	/*
	 * in case of setting gamma and panel condition at first,
	 * it shuold be setting like below.
	 * set_gamma() -> set_panel_condition()
	 */

	s6e8ax0_apply_level1_key(priv);
	s6e8ax0_apply_mtp_key(priv);

	s6e8ax0_sleep_out(priv);
	mdelay(5);
	s6e8ax0_panel_cond(priv);
	s6e8ax0_display_cond(priv);
	s6e8ax0_gamma_cond(priv);
	s6e8ax0_gamma_update(priv);

	s6e8ax0_etc_source_control(priv);
	s6e8ax0_elvss_set(priv);
	s6e8ax0_etc_pentile_control(priv);
	s6e8ax0_etc_mipi_control1(priv);
	s6e8ax0_etc_mipi_control2(priv);
	s6e8ax0_etc_power_control(priv);
	s6e8ax0_etc_mipi_control3(priv);
	s6e8ax0_etc_mipi_control4(priv);
}

static int s6e8ax0_panel_set(struct s6e8ax0_priv *priv)
{
	s6e8ax0_panel_init(priv);

	return 0;
}

static int s6e8ax0_display_enable(struct udevice *dev)
{
	struct s6e8ax0_priv *priv = dev_get_priv(dev);
	int ret;

	ret = regulator_set_enable(priv->vci_reg, 1);
	if (ret) {
		return ret;
	}

	ret = regulator_set_enable(priv->vdd3_reg, 1);
	if (ret) {
		return ret;
	}

	mdelay(100);

	dm_gpio_set_value(&priv->reset_gpio, 0);
	udelay(11000);
	dm_gpio_set_value(&priv->reset_gpio, 1);

	mdelay(100);

	s6e8ax0_panel_set(priv);
	s6e8ax0_display_on(priv);
	return 0;
}

static int s6e8ax0_ofdata_to_platdata(struct udevice *dev)
{
	struct s6e8ax0_priv *priv = dev_get_priv(dev);
	int ret;

	ret = uclass_get_device_by_phandle(UCLASS_REGULATOR, dev,
			"vci-supply", &priv->vci_reg);
	if (ret) {
		printf("%s: Couldn't get vci: %d\n", __func__, ret);
		return ret;
	}

	ret = uclass_get_device_by_phandle(UCLASS_REGULATOR, dev,
			"vdd3-supply", &priv->vdd3_reg);
	if (ret) {
		printf("%s: Couldn't get vdd3: %d\n", __func__, ret);
		return ret;
	}

	ret = gpio_request_by_name(dev, "reset-gpios", 0, &priv->reset_gpio,
			GPIOD_IS_OUT);
	if (ret) {
		printf("%s: Couldn't get reset GPIO: %d\n", __func__, ret);
		return ret;
	}

	printf("Loaded s6e8ax0 ok\n");
	return 0;
}

static int s6e8ax0_probe(struct udevice *dev)
{
	int ret;
	struct s6e8ax0_priv *priv = dev_get_priv(dev);
	ret = uclass_get_device_by_phandle(UCLASS_DISPLAY, dev,
			"dsi-master", &priv->display);
	if (ret) {
		printf("%s: Couldn't get DSI master: %d\n", __func__, ret);
		return ret;
	}
	return 0;
}

static const struct panel_ops s6e8ax0_panel_ops = {
	.enable_backlight = s6e8ax0_display_enable,
};

static const struct udevice_id s6e8ax0_panel_ids[] = {
	{ .compatible = "samsung,s6e8ax0" },
	{ },
};

U_BOOT_DRIVER(s6e8ax0) = {
	.name = "s6e8ax0",
	.id = UCLASS_PANEL,
	.of_match = s6e8ax0_panel_ids,
	.ops = &s6e8ax0_panel_ops,
	.ofdata_to_platdata = s6e8ax0_ofdata_to_platdata,
	.probe = s6e8ax0_probe,
	.priv_auto_alloc_size	= sizeof(struct s6e8ax0_priv),
};
