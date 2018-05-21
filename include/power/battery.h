/* SPDX-License-Identifier: GPL-2.0+ */
/*
 *  Copyright (C) 2012 Samsung Electronics
 *  Lukasz Majewski <l.majewski@samsung.com>
 */

#ifndef __POWER_BATTERY_H_
#define __POWER_BATTERY_H_

#ifndef CONFIG_DM_BATTERY
/* TODO: remove this once all users have been moved over to DM */
struct battery {
	unsigned int version;
	unsigned int state_of_chrg;
	unsigned int time_to_empty;
	unsigned int capacity;
	unsigned int voltage_uV;

	unsigned int state;
};

int power_bat_init(unsigned char bus);
#else

enum dm_battery_state {
	BAT_STATE_UNUSED = 0, /* never used */
	BAT_STATE_NOT_PRESENT, /* battery is not present */
	BAT_STATE_NEED_CHARGING, /* battery needs charging (i.e. low SOC or voltage) */
	BAT_STATE_NORMAL, /* battery is OK */
};

/* Battery device operations */
struct dm_battery_ops {
	/**
	 * Get the current voltage of the battery.
	 * @dev		- battery device
	 * @uV		- pointer to place to store voltage, in microvolts
	 * @return 0 if success, -errno otherwise.
	 */
	int (*get_voltage)(struct udevice *dev, unsigned int *uV);
	/**
	 * Get the current battery status
	 * @dev		- battery device
	 * @return -errno on error, enum dm_battery_state otherwise
	 */
	int (*get_status)(struct udevice *dev);
	/**
	 * Get the battery's State Of Charge (SOC)
	 * @dev		- battery device
	 * @return 0-100 value representing current battery charge percentage, -errno on error
	 */
	int (*get_soc)(struct udevice *dev);
};

int battery_get(const char *devname, struct udevice **devp);
int battery_get_voltage(struct udevice *dev, unsigned int *uV);
int battery_get_status(struct udevice *dev);
int battery_get_soc(struct udevice *dev);
#endif /* CONFIG_DM_BATTERY */
#endif /* __POWER_BATTERY_H_ */
