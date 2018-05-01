/*
 * Copyright (C) 2018 Simon Shields <simon@lineageos.org>
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <common.h>
#include <fdtdec.h>
#include <errno.h>
#include <dm.h>
#include <i2c.h>
#include <power/pmic.h>
#include <power/max77693_pmic.h>
#include <power/max77693_muic.h>

DECLARE_GLOBAL_DATA_PTR;

#define PMIC_TYPE_CHARGER 0
#define PMIC_TYPE_MUIC 1
#define PMIC_TYPE_HAPTIC 2

static const struct pmic_child_info pmic_children_info[] = {
	{ .prefix = "ESAFEOUT", .driver = MAX77693_SAFEOUT_DRIVER },
	{ .prefix = "CHARGER", .driver = MAX77693_CHARGER_DRIVER },
	{ .prefix = "muic", .driver = MAX77693_MUIC_DRIVER },
};

static int max77693_reg_count(struct udevice *dev)
{
	ulong type = dev_get_driver_data(dev);

	switch (type) {
	case PMIC_TYPE_CHARGER:
		return PMIC_NUM_OF_REGS;
	case PMIC_TYPE_MUIC:
		return MUIC_NUM_OF_REGS;
	case PMIC_TYPE_HAPTIC:
		// TODO
		return 0;
	}

	return -ENODEV;
}

static int max77693_write(struct udevice *dev, uint reg, const uint8_t *buff, int len)
{
	int ret;

	ret = dm_i2c_write(dev, reg, buff, len);
	if (ret)
		pr_err("%s: %p: error writing to register %#x!", __func__, dev, reg);

	return ret;
}

static int max77693_read(struct udevice *dev, uint reg, uint8_t *buff, int len)
{
	int ret;

	ret = dm_i2c_read(dev, reg, buff, len);
	if (ret)
		pr_err("%s: %p: error reading from register %#x!", __func__, dev, reg);

	return ret;
}

static int max77693_bind(struct udevice *dev)
{
	ofnode children_node;
	int children;
	ulong type = dev_get_driver_data(dev);

	if (type == PMIC_TYPE_MUIC) {
		children_node = dev_ofnode(dev);
	} else {
		children_node = dev_read_subnode(dev, "regulators");
		if (!ofnode_valid(children_node)) {
			debug("%s: %s: found no regulators!", dev->name,
					__func__);
			return -ENXIO;
		}
	}

	children = pmic_bind_children(dev, children_node, pmic_children_info);
	if (!children)
		debug("%s: %s: found no children!", dev->name,
				__func__);

	return 0;
}

static struct dm_pmic_ops max77693_ops = {
	.reg_count = max77693_reg_count,
	.read = max77693_read,
	.write = max77693_write,
};

static const struct udevice_id max77693_ids[] = {
	{ .compatible = "maxim,max77693-charger", .data = PMIC_TYPE_CHARGER },
	{ .compatible = "maxim,max77693-muic",  .data = PMIC_TYPE_MUIC },
	{ }
};


U_BOOT_DRIVER(pmic_max77693) = {
	.name = "max77693_pmic",
	.id = UCLASS_PMIC,
	.of_match = max77693_ids,
	.bind = max77693_bind,
	.ops = &max77693_ops,
};
