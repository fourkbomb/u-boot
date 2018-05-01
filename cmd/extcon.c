/*
 * Copyright (C) 2018 Simon Shields <simon@lineageos.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <errno.h>
#include <dm.h>
#include <dm/uclass-internal.h>
#include <extcon.h>

static struct udevice *currdev;

#define LIMIT_DEV	32
#define LIMIT_PARENT	20

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
		ret = extcon_get(name, &currdev);
		if (ret) {
			printf("Can't get extcon: %s!\n", name);
			return failure(ret);
		}
	case 1:
		if (!currdev) {
			printf("Extcon device is not set!\n\n");
			return CMD_RET_USAGE;
		}

		printf("dev: %d @ %s\n", currdev->seq, currdev->name);
		printf("current cable type: %s\n", extcon_get_cable_name(extcon_get_cable_id(currdev, NULL)));
		printf("current max charge speed: %d uA\n", extcon_get_max_charge_current(currdev));
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

	for (ret = uclass_first_device(UCLASS_EXTCON, &dev); dev;
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


static cmd_tbl_t subcmd[] = {
	U_BOOT_CMD_MKENT(dev, 2, 1, do_dev, "", ""),
	U_BOOT_CMD_MKENT(list, 1, 1, do_list, "", ""),
};

static int do_extcon(cmd_tbl_t *cmdtp, int flag, int argc,
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


U_BOOT_CMD(extcon, CONFIG_SYS_MAXARGS, 1, do_extcon,
		"Extcon subsystem",
		"list			- list extcon devices\n"
		"extcon dev [name]	- show or [set] operating PMIC device\n"
);
