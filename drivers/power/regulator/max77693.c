/*
 * Copyright (C) 2018 Simon Shields <simon@lineageos.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <fdtdec.h>
#include <errno.h>
#include <dm.h>
#include <i2c.h>
#include <power/pmic.h>
#include <power/regulator.h>
#include <power/max77693_pmic.h>

DECLARE_GLOBAL_DATA_PTR;

enum max77693_reg_type {
	CHARGER = -1,
	SAFEOUT1 = 1,
	SAFEOUT2 = 2,
};

static const int max77693_safeout_voltages[] = {
	4850000, 4900000, 4950000, 3300000,
};

#define CHARGER_MIN 60000
#define CHARGER_MAX 2580000
#define CHARGER_STEP 20000

static int max77693_reg_get_enable_register(enum max77693_reg_type type,
		int *reg, int *mask, int *pattern)
{
	switch (type) {
	case CHARGER:
		*reg = MAX77693_CHG_CNFG_00;
		*mask = 0x5;
		*pattern = 0x5;
		break;
	case SAFEOUT1:
	case SAFEOUT2:
		*reg = MAX77693_SAFEOUT;
		*mask = 0x40 << (type - SAFEOUT1);
		*pattern = 0x40 << (type - SAFEOUT1);
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static int max77693_reg_get_enable(struct udevice *dev)
{
	int reg, mask, pattern, ret;
	unsigned char val;

	ret = max77693_reg_get_enable_register(dev->driver_data,
				&reg, &mask, &pattern);
	if (ret) {
		pr_err("%s: bad regulator ID %ld\n", __func__, dev->driver_data);
		return ret;
	}

	ret = pmic_read(dev->parent, reg, &val, 1);
	if (ret)
		return ret;

	return (val & mask) == pattern;
}

static int max77693_reg_set_enable(struct udevice *dev, bool enable)
{
	int reg, mask, pattern, ret;
	unsigned char val;

	ret = max77693_reg_get_enable_register(dev->driver_data,
				&reg, &mask, &pattern);
	if (ret) {
		pr_err("%s: bad regulator ID %ld\n", __func__, dev->driver_data);
		return ret;
	}

	ret = pmic_read(dev->parent, reg, &val, 1);
	if (ret)
		return ret;

	val &= ~mask;
	if (enable) {
		val |= pattern;
	}

	ret = pmic_write(dev->parent, reg, &val, 1);
	return ret;
}

static int max77693_safeout_get_value(struct udevice *dev)
{
	int ret;
	int mask = 0x3;
	int shift = 0;
	unsigned char val;

	ret = pmic_read(dev->parent, MAX77693_SAFEOUT, &val, 1);
	if (ret) {
		pr_err("%s: couldn't read safeout reg: %d\n", __func__, ret);
		return ret;
	}

	if (dev->driver_data == SAFEOUT2)
		shift = 2;

	val >>= shift;
	val &= mask;

	if (val < ARRAY_SIZE(max77693_safeout_voltages))
		return max77693_safeout_voltages[val];

	return -EINVAL;
}

static int max77693_safeout_set_value(struct udevice *dev, int uV)
{
	int ret;
	int mask = 0x3;
	int shift = 0;
	int index = -1;
	unsigned char val;

	for (int i = 0; i < ARRAY_SIZE(max77693_safeout_voltages); i++) {
		if (max77693_safeout_voltages[i] == uV) {
			index = i;
			break;
		}
	}

	if (index < 0)
		return -EINVAL;

	ret = pmic_read(dev->parent, MAX77693_SAFEOUT, &val, 1);
	if (ret) {
		pr_err("%s: couldn't read safeout reg: %d\n", __func__, ret);
		return ret;
	}

	if (dev->driver_data == SAFEOUT2)
		shift = 2;

	val &= ~(mask << shift);
	val |= index << shift;

	ret = pmic_write(dev->parent, MAX77693_SAFEOUT, &val, 1);
	return ret;
}

static int max77693_charger_get_current(struct udevice *dev)
{
	int ret;
	unsigned char val;

	ret = pmic_read(dev->parent, MAX77693_CHG_CNFG_09, &val, 1);
	if (ret) {
		pr_err("%s: couldn't read charger reg: %d\n", __func__, ret);
		return ret;
	}

	val &= 0x7f;

	/* the first four values are all 60mA */
	if (val <= 3)
		val = 0;
	else
		val -= 3;

	ret = CHARGER_MIN + val * CHARGER_STEP;

	if (ret > CHARGER_MAX)
		pr_err("%s: invalid value %d\n", __func__, ret);

	return ret;
}

static int max77693_charger_set_current(struct udevice *dev, int uA)
{
	unsigned char sel = 0;
	int ret;
	unsigned char data;

	while (CHARGER_MIN + CHARGER_STEP * sel < uA)
		sel++;

	if (CHARGER_MIN + CHARGER_STEP * sel != uA || uA > CHARGER_MAX)
		return -EINVAL;

	/* the first four values are all 60mA */
	sel += 3;

	ret = pmic_write(dev->parent, MAX77693_CHG_CNFG_09, &sel, 1);
	if (ret) {
		pr_err("%s: linear charge speed config failed: %d\n", __func__, ret);
		return ret;
	}

	/* fast charging */
	ret = pmic_read(dev->parent, MAX77693_CHG_CNFG_02, &data, 1);
	if (ret) {
		pr_err("%s: fast charge speed read failed: %d\n", __func__, ret);
		return ret;
	}


	/* uA -> mA */
	uA /= 1000;
	/* adjust for the register */
	uA *= 3;
	uA /= 100;

	data &= ~0x3f;
	data |= uA;

	ret = pmic_write(dev->parent, MAX77693_CHG_CNFG_02, &data, 1);
	return ret;
}

static const struct dm_regulator_ops max77693_charger_ops = {
	.get_current = max77693_charger_get_current,
	.set_current = max77693_charger_set_current,
	.set_enable = max77693_reg_set_enable,
	.get_enable = max77693_reg_get_enable,
};

U_BOOT_DRIVER(max77693_reg_charger) = {
	.name = MAX77693_REGULATOR_CHARGER_DRIVER,
	.id = UCLASS_REGULATOR,
	.ops = &max77693_charger_ops,
};

static const struct dm_regulator_ops max77693_safeout_ops = {
	.get_value = max77693_safeout_get_value,
	.set_value = max77693_safeout_set_value,
	.set_enable = max77693_reg_set_enable,
	.get_enable = max77693_reg_get_enable,
};

U_BOOT_DRIVER(max77693_reg_safeout) = {
	.name = MAX77693_REGULATOR_SAFEOUT_DRIVER,
	.id = UCLASS_REGULATOR,
	.ops = &max77693_safeout_ops,
};
