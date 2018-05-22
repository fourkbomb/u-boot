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
#include <power/charger.h>
#include <power/max77693_pmic.h>
#include <power/pmic.h>
#include <power/regulator.h>

DECLARE_GLOBAL_DATA_PTR;

enum max77693_charger_charging_state {
	MAX77693_CHARGING_PREQUALIFICATION	= 0x0,
	MAX77693_CHARGING_FAST_CONST_CURRENT,
	MAX77693_CHARGING_FAST_CONST_VOLTAGE,
	MAX77693_CHARGING_TOP_OFF,
	MAX77693_CHARGING_DONE,
	MAX77693_CHARGING_HIGH_TEMP,
	MAX77693_CHARGING_TIMER_EXPIRED,
	MAX77693_CHARGING_THERMISTOR_SUSPEND,
	MAX77693_CHARGING_OFF,
	MAX77693_CHARGING_RESERVED,
	MAX77693_CHARGING_OVER_TEMP,
	MAX77693_CHARGING_WATCHDOG_EXPIRED,
};

static int max77693_get_current(struct udevice *dev)
{
	return regulator_get_current(dev->priv);
}

static int max77693_set_current(struct udevice *dev, unsigned int microamps)
{
	struct udevice *reg = dev->priv;
	int ret;

	if (!microamps) {
		ret = regulator_set_current(reg, 0);
		if (ret) {
			printf("regulator_set_current(0) failed: %d\n", ret);
			return ret;
		}

		ret = regulator_set_enable(reg, 0);
		if (ret) {
			printf("failed to disable charger regulator: %d\n", ret);
			return ret;
		}
	} else {
		ret = regulator_set_current(reg, microamps);
		if (ret) {
			printf("regulator_set_current(%u) failed: %d\n", microamps, ret);
			return ret;
		}

		ret = regulator_set_enable(reg, 1);
		if (ret) {
			printf("failed to enable charger regulator: %d\n", ret);
			return ret;
		}
	}

	return 0;
}

static int max77693_get_status(struct udevice *dev)
{
	u8 reg;
	int ret = pmic_read(dev->parent, MAX77693_CHG_DETAILS_01, &reg, 1);
	if (ret) {
		printf("Failed to read CHG_DETAILS_01: %d\n", ret);
		return ret;
	}

	reg &= 0xf;

	switch (reg) {
	case MAX77693_CHARGING_PREQUALIFICATION:
	case MAX77693_CHARGING_FAST_CONST_CURRENT:
	case MAX77693_CHARGING_FAST_CONST_VOLTAGE:
	case MAX77693_CHARGING_TOP_OFF:
	case MAX77693_CHARGING_HIGH_TEMP:
		return CHARGE_STATE_CHARGING;
	case MAX77693_CHARGING_DONE:
		return CHARGE_STATE_FULL;
	case MAX77693_CHARGING_TIMER_EXPIRED:
	case MAX77693_CHARGING_THERMISTOR_SUSPEND:
		return CHARGE_STATE_NOT_CHARGING;
	case MAX77693_CHARGING_OFF:
	case MAX77693_CHARGING_OVER_TEMP:
	case MAX77693_CHARGING_WATCHDOG_EXPIRED:
		return CHARGE_STATE_DISCHARGING;
	}

	return CHARGE_STATE_UNKNOWN;
}

static int max77693_probe(struct udevice *dev)
{
	int ret = device_get_supply_regulator(dev, "charger-supply", &dev->priv);
	if (ret) {
		printf("Failed to get charger regulator: %d\n", ret);
		return ret;
	}

	return 0;
}

static struct dm_charger_ops max77693_chg_ops = {
	.get_current = max77693_get_current,
	.set_current = max77693_set_current,
	.get_status = max77693_get_status,
};

U_BOOT_DRIVER(charger_max77693) = {
	.name = MAX77693_CHARGER_DRIVER,
	.id = UCLASS_CHARGER,
	.ops = &max77693_chg_ops,
	.probe = max77693_probe,
};
