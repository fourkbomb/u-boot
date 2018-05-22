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
#include <power/max17047.h>

static const struct pmic_child_info pmic_children_info[] = {
	{ .prefix = "fuelgauge", .driver = MAX17047_FUELGAUGE_DRIVER },
};

static int max17047_reg_count(struct udevice *dev)
{
	return FG_NUM_OF_REGS;
};

static int max17047_write(struct udevice *dev, uint reg, const uint8_t *buff, int len)
{
	int ret;

	ret = dm_i2c_write(dev, reg, buff, len);
	if (ret)
		pr_err("%s: %p: error writing to register %#x!", __func__, dev, reg);

	return ret;
}

static int max17047_read(struct udevice *dev, uint reg, uint8_t *buff, int len)
{
	int ret;

	ret = dm_i2c_read(dev, reg, buff, len);
	if (ret)
		pr_err("%s: %p: error reading from register %#x!", __func__, dev, reg);

	return ret;
}

static int max17047_bind(struct udevice *dev)
{
	ofnode children_node;
	int children;

	children_node = dev_ofnode(dev);

	children = pmic_bind_children(dev, children_node, pmic_children_info);
	if (!children)
		pr_err("%s: %s: found no children!", dev->name, __func__);

	return 0;
}

static struct dm_pmic_ops max17047_ops = {
	.reg_count = max17047_reg_count,
	.read = max17047_read,
	.write = max17047_write,
};

static const struct udevice_id max17047_ids[] = {
	{ .compatible = "maxim,max17047-fuelgauge" },
	{ }
};

U_BOOT_DRIVER(pmic_max17047) = {
	.name = "max17047_pmic",
	.id = UCLASS_PMIC,
	.of_match = max17047_ids,
	.bind = max17047_bind,
	.ops = &max17047_ops,
};
