/*
 * i9300 board file
 * Copyright (C) 2018 Simon Shields <simon@lineageos.org>
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <common.h>

DECLARE_GLOBAL_DATA_PTR;

int exynos_init(void)
{
	gd->ram_size = 0x4000000;
	return 0;
}
