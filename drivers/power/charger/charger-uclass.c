/*
 * Copyright (C) 2018 Simon Shields <simon@lineageos.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <errno.h>
#include <dm.h>
#include <dm/uclass-internal.h>
#include <power/charger.h>

DECLARE_GLOBAL_DATA_PTR;

int charger_get(const char *devname, struct udevice **devp)
{
	return uclass_get_device_by_name(UCLASS_CHARGER, devname, devp);
}

int charger_set_current(struct udevice *dev, unsigned int microamps)
{
	const struct dm_charger_ops *ops = dev_get_driver_ops(dev);

	if (!ops || !ops->set_current)
		return -ENOSYS;

	return ops->set_current(dev, microamps);
}

int charger_get_current(struct udevice *dev)
{
	const struct dm_charger_ops *ops = dev_get_driver_ops(dev);

	if (!ops || !ops->get_current)
		return -ENOSYS;

	return ops->get_current(dev);
}

int charger_get_status(struct udevice *dev)
{
	const struct dm_charger_ops *ops = dev_get_driver_ops(dev);

	if (!ops || !ops->get_status)
		return -ENOSYS;

	return ops->get_status(dev);
}


UCLASS_DRIVER(charger) = {
	.id = UCLASS_CHARGER,
	.name = "charger",
};
