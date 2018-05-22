/*
 * Copyright (C) 2018 Simon Shields <simon@lineageos.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

enum charger_state {
	CHARGE_STATE_UNKNOWN = 0,
	CHARGE_STATE_CHARGING = 1, /* charging normally */
	CHARGE_STATE_FULL = 2, /* not charging - battery full */
	CHARGE_STATE_NOT_CHARGING = 3, /* not charging - some other reason */
	CHARGE_STATE_DISCHARGING = 4, /* discharging */
};

struct dm_charger_ops {
	/**
	 * Get the charge current of the charger.
	 * Some devices may return the maximum charge current rather than the current charge current.
	 * @dev		- charger device.
	 * @return -errno on error, charge current in uA.
	 */
	int (*get_current)(struct udevice *dev);
	/**
	 * Set the maximum charge current for the charger. A current of zero will disable charging.
	 * @dev		- charger device
	 * @return	-errno on error, 0 otherwise.
	 */
	int (*set_current)(struct udevice *dev, unsigned int microamps);
	/**
	 * Get current charging state.
	 * @dev		- charger device
	 * @return	-errno on error, enum charger_state otherwise.
	 */
	int (*get_status)(struct udevice *dev);
};

int charger_get(const char *devname, struct udevice **devp);
int charger_get_current(struct udevice *dev);
int charger_set_current(struct udevice *dev, unsigned int microamps);
int charger_get_status(struct udevice *dev);
