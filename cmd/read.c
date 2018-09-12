/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 */

#include <common.h>
#include <command.h>
#include <part.h>

int do_read(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char *ep;
	struct blk_desc *dev_desc = NULL;
	int dev;
	int addr_index = argc == 6 ? 3 : 4;
	int part = 0;
	disk_partition_t part_info;
	ulong offset = 0u;
	ulong limit = 0u;
	void *addr;
	uint blk;
	uint cnt;

	if (argc != 6 && argc != 7) {
		cmd_usage(cmdtp);
		return 1;
	}

	dev = (int)simple_strtoul(argv[2], &ep, 16);
	if (*ep) {
		if (argc == 7 || *ep != ':') {
			printf("Invalid block device %s\n", argv[2]);
			return 1;
		}
		part = (int)simple_strtoul(++ep, NULL, 16);
	}

	dev_desc = blk_get_dev(argv[1], dev);
	if (dev_desc == NULL) {
		printf("Block device %s %d not supported\n", argv[1], dev);
		return 1;
	}

	if (argc == 7) {
		part = part_get_info_by_name(dev_desc, argv[3], &part_info);
		if (part < 1) {
			printf("Partition %s not found\n", argv[3]);
			return 1;
		}
	}

	addr = (void *)simple_strtoul(argv[addr_index], NULL, 16);
	blk = simple_strtoul(argv[addr_index+1], NULL, 16);
	if (part != 0) {
		if (part_get_info(dev_desc, part, &part_info)) {
			printf("Cannot find partition %d\n", part);
			return 1;
		}
		offset = part_info.start;
		limit = part_info.size;
	} else {
		/* Largest address not available in struct blk_desc. */
		limit = ~0;
	}

	if (!strcmp("end", argv[addr_index+2]))
		cnt = limit - blk;
	else
		cnt = simple_strtoul(argv[addr_index+2], NULL, 16);

	if (cnt + blk > limit) {
		printf("Read out of range\n");
		return 1;
	}

	if (blk_dread(dev_desc, offset + blk, cnt, addr) != cnt) {
		printf("Error reading blocks\n");
		return 1;
	}

	return 0;
}

U_BOOT_CMD(
	read,	7,	0,	do_read,
	"Load binary data from a partition",
	"<interface> <dev[:part]> addr blk# cnt\n"
	"read <interface> <dev> <partlabel> addr blk# cnt - use 'end' in place of cnt to read the whole partition"
);
