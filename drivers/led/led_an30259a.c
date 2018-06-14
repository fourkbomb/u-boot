/*
 * Copyright (C) 2018 Simon Shields <simon@lineageos.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <errno.h>
#include <i2c.h>
#include <led.h>
#include <asm/io.h>
#include <dm/lists.h>

#define LED_MAX 3

#define REG_SRESET 0x0
#define SRESET (1 << 0)

#define REG_LEDON 0x1
#define LEDON(x) (1 << (x))

#define REG_LEDCC(x) (0x3 + (x))
#define REG_LEDCC_ON (0xff)

struct an30259a_led_priv {
	int index;
};

static int an30259a_write(struct udevice *dev, uint reg, uint8_t val)
{
	struct led_uc_plat *uc_plat = dev_get_uclass_platdata(dev);
	struct udevice *i2c_dev = dev;
	if (uc_plat->label)
		i2c_dev = dev->parent;

	return dm_i2c_write(i2c_dev, reg, &val, 1);
}

static int an30259a_read(struct udevice *dev, uint reg, uint8_t *val)
{
	struct led_uc_plat *uc_plat = dev_get_uclass_platdata(dev);
	struct udevice *i2c_dev = dev;
	if (uc_plat->label)
		i2c_dev = dev->parent;

	return dm_i2c_read(i2c_dev, reg, val, 1);
}

static int an30259a_set_state(struct udevice *dev, enum led_state_t state)
{
	struct an30259a_led_priv *priv = dev_get_priv(dev);
	u8 ledon;
	int ret;

	ret = an30259a_read(dev, REG_LEDON, &ledon);
	if (ret) {
		return ret;
	}

	switch (state) {
	case LEDST_OFF:
		ledon &= ~LEDON(priv->index);
		break;
	case LEDST_ON:
		ledon |= LEDON(priv->index);
		break;
	case LEDST_TOGGLE:
		ledon ^= LEDON(priv->index);
		break;
	default:
		return -EINVAL;
	}

	return an30259a_write(dev, REG_LEDON, ledon);
}

static enum led_state_t an30259a_get_state(struct udevice *dev)
{
	struct an30259a_led_priv *priv = dev_get_priv(dev);
	u8 ledon;
	int ret;

	ret = an30259a_read(dev, REG_LEDON, &ledon);
	if (ret) {
		return ret;
	}

	if (ledon & LEDON(priv->index))
		return LEDST_ON;

	return LEDST_OFF;
}


static struct led_ops an30259a_led_ops = {
	.set_state = an30259a_set_state,
	.get_state = an30259a_get_state,
};

static int an30259a_led_probe(struct udevice *dev)
{
	struct led_uc_plat *uc_plat = dev_get_uclass_platdata(dev);
	int ret;

	if (!uc_plat->label) {
		printf("Probe parent\n");
		/* reset the controller */
		an30259a_write(dev, REG_SRESET, SRESET);
		for (int i = 0; i < LED_MAX; i++) {
			/* This value is only used if the corresponding LEDON bit is set. */
			ret = an30259a_write(dev, REG_LEDCC(i), REG_LEDCC_ON);
			if (ret) {
				printf("%d failed: %d\n", i, ret);
			}
		}
		printf("(OK)\n");
	} else {
		struct an30259a_led_priv *priv = dev_get_priv(dev);
		printf("Probe %s\n", uc_plat->label);
		/* To match the datasheet, address in DT is 1-3 inclusive */
		u32 addr;
		ret = dev_read_u32(dev, "reg", &addr);
		if (ret < 0)
			return ret;
		if (addr > LED_MAX || addr == 0)
			return -EINVAL;

		priv->index = addr - 1;
	}

	return 0;
}

static int an30259a_led_bind(struct udevice *parent)
{
	ofnode parent_ofnode = dev_ofnode(parent);
	ofnode node;

	ofnode_for_each_subnode(node, parent_ofnode) {
		struct udevice *dev;
		const char *label;
		struct led_uc_plat *uc_plat;
		int ret;

		label = ofnode_read_string(node, "label");
		if (!label) {
			printf("%s: node %s has no label\n", __func__,
					ofnode_get_name(node));
			return -EINVAL;
		}

		ret = device_bind_driver_to_node(parent, "an30259a_led",
				ofnode_get_name(node), node, &dev);
		if (ret) {
			printf("%s: probe of %s failed: %d\n", __func__, ofnode_get_name(node), ret);
			return ret;
		}

		printf("%s: bound %s\n", __func__, label);
		uc_plat = dev_get_uclass_platdata(dev);
		uc_plat->label = label;
	}

	return 0;
}

static const struct udevice_id an30259a_led_ids[] = {
	{ .compatible = "panasonic,an30259a" },
	{ /* sentinel */ }
};

U_BOOT_DRIVER(an30259a_led) = {
	.name = "an30259a_led",
	.id = UCLASS_LED,
	.of_match = an30259a_led_ids,
	.ops = &an30259a_led_ops,
	.bind = an30259a_led_bind,
	.probe = an30259a_led_probe,
	.priv_auto_alloc_size = sizeof(struct an30259a_led_priv),
};

