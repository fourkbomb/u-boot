/*
 * Copyright (C) 2018 Simon Shields <simon@lineageos.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <errno.h>
#include <dm.h>
#include <dm/uclass-internal.h>
#include <power/battery.h>

DECLARE_GLOBAL_DATA_PTR;

int battery_get(const char *devname, struct udevice **devp)
{
	return uclass_get_device_by_name(UCLASS_BATTERY, devname, devp);
}

int battery_get_voltage(struct udevice *dev, unsigned int *uV)
{
	const struct dm_battery_ops *ops = dev_get_driver_ops(dev);

	if (!ops || !ops->get_voltage)
		return -ENOSYS;

	return ops->get_voltage(dev, uV);
}

int battery_get_status(struct udevice *dev)
{
	const struct dm_battery_ops *ops = dev_get_driver_ops(dev);

	if (!ops || !ops->get_status)
		return -ENOSYS;

	return ops->get_status(dev);
}

int battery_get_soc(struct udevice *dev)
{
	const struct dm_battery_ops *ops = dev_get_driver_ops(dev);

	if (!ops || !ops->get_soc)
		return -ENOSYS;

	return ops->get_soc(dev);
}

UCLASS_DRIVER(battery) = {
	.id = UCLASS_BATTERY,
	.name = "battery",
};
