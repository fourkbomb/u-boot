/*
 * i9300 board file
 * Copyright (C) 2018 Simon Shields <simon@lineageos.org>
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <common.h>
#include <asm/gpio.h>
#include <asm/arch/gpio.h>

DECLARE_GLOBAL_DATA_PTR;

int get_board_rev(void)
{
	// TODO: GPM1[5:2]
	return 0;
}

static void board_gpio_init(void)
{
	/* GPX2[7] - power key */
	gpio_request(EXYNOS4X12_GPIO_X27, "nPOWER");
	gpio_cfg_pin(EXYNOS4X12_GPIO_X27, S5P_GPIO_INPUT);
	gpio_set_pull(EXYNOS4X12_GPIO_X27, S5P_GPIO_PULL_NONE);
}

int exynos_init(void)
{
	board_gpio_init();

	return 0;
}
