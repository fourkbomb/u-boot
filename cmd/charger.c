/*
 * Copyright (C) 2018 Simon Shields <simon@lineageos.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <errno.h>
#include <dm.h>
#include <dm/uclass-internal.h>
#include <console.h>
#include <power/battery.h>
#include <power/charger.h>
#include <extcon.h>

static struct udevice *currdev;

#define LIMIT_DEV	32
#define LIMIT_PARENT	20

static const char *chg_states[] = {
	[CHARGE_STATE_UNKNOWN] = "Unknown",
	[CHARGE_STATE_CHARGING] = "Charging",
	[CHARGE_STATE_FULL] = "Battery full",
	[CHARGE_STATE_NOT_CHARGING] = "Not charging",
	[CHARGE_STATE_DISCHARGING] = "Discharging",
};

static int failure(int ret)
{
	printf("Error: %d (%s)\n", ret, errno_str(ret));

	return CMD_RET_FAILURE;
}

static int do_dev(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	int ret = -ENODEV;
	char *name;

	switch (argc) {
	case 2:
		name = argv[1];
		ret = charger_get(name, &currdev);
		if (ret) {
			printf("Can't get charger: %s!\n", name);
			return failure(ret);
		}
	case 1:
		if (!currdev) {
			printf("Charger device is not set!\n\n");
			return CMD_RET_USAGE;
		}

		printf("dev: %d @ %s\n", currdev->seq, currdev->name);
		ret = charger_get_current(currdev);
		if (ret < 0)
			printf("failed to get current: %d\n", ret);
		printf("current: %d uA\n", ret);
		ret = charger_get_status(currdev);
		if (ret < 0)
			printf("failed to get charger status: %d\n", ret);
		else
			printf("charger status: %s\n", chg_states[ret]);
	}

	return CMD_RET_SUCCESS;
}

static int do_list(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct udevice *dev;
	int ret;

	printf("| %-*.*s| %-*.*s| %s @ %s\n",
	       LIMIT_DEV, LIMIT_DEV, "Name",
	       LIMIT_PARENT, LIMIT_PARENT, "Parent name",
	       "Parent uclass", "seq");

	for (ret = uclass_first_device(UCLASS_CHARGER, &dev); dev;
	     ret = uclass_next_device(&dev)) {
		if (ret)
			continue;

		printf("| %-*.*s| %-*.*s| %s @ %d\n",
		       LIMIT_DEV, LIMIT_DEV, dev->name,
		       LIMIT_PARENT, LIMIT_PARENT, dev->parent->name,
		       dev_get_uclass_name(dev->parent), dev->parent->seq);
	}

	if (ret)
		return CMD_RET_FAILURE;

	return CMD_RET_SUCCESS;
}

static int do_wait(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct udevice *extcon, *battery;
	int ret = extcon_get(argv[1], &extcon); 
	if (ret) {
		printf("Failed to get extcon '%s': %d\n", argv[1], ret);
		return failure(ret);
	}

	ret = battery_get(argv[2], &battery);
	if (ret) {
		printf("Failed to get battery\n");
		return failure(ret);
	}

	if (!currdev) {
		printf("No currdev!\n");
		return CMD_RET_FAILURE;
	}

	printf("Waiting...\n");
	int current = -1;
	int status = -1;
	int start_soc = battery_get_soc(battery);
	u32 start_voltage;
	int fini_soc;
	u32 fini_voltage;
	battery_get_voltage(battery, &start_voltage);
	ret = 42;
	while (1) {
		current = extcon_get_max_charge_current(extcon);
		if (current)
			break;
		mdelay(500);
	}

	if (current < 0) {
		goto end;
	}

	ret = charger_set_current(currdev, current);

	mdelay(500);
	while (1) {
		current = charger_get_current(currdev);
		fini_soc = battery_get_soc(battery);
		battery_get_voltage(battery, &fini_voltage);
		status = charger_get_status(currdev);
		mdelay(2000);

		printf("SOC: %d=>%d, voltage:%u->%u, current:%d, status:%d\n", start_soc, fini_soc,
				start_voltage, fini_voltage, current, status);
		if (ctrlc())
			break;
	}

end:
	return CMD_RET_SUCCESS;
}

static cmd_tbl_t subcmd[] = {
	U_BOOT_CMD_MKENT(dev, 2, 1, do_dev, "", ""),
	U_BOOT_CMD_MKENT(list, 1, 1, do_list, "", ""),
	U_BOOT_CMD_MKENT(wait, 3, 3, do_wait, "", ""),
};

static int do_charger(cmd_tbl_t *cmdtp, int flag, int argc,
			char * const argv[])
{
	cmd_tbl_t *cmd;

	argc--;
	argv++;

	cmd = find_cmd_tbl(argv[0], subcmd, ARRAY_SIZE(subcmd));
	if (cmd == NULL || argc > cmd->maxargs)
		return CMD_RET_USAGE;

	return cmd->cmd(cmdtp, flag, argc, argv);
}


U_BOOT_CMD(charger, CONFIG_SYS_MAXARGS, 1, do_charger,
		"Charger subsystem",
		"list			- list charger devices\n"
		"dev [name]	- show or [set] operating charger device\n"
		"wait <extcon> <battery>	- wait for extcon cable connection & automatically enable charging.\n"
);
