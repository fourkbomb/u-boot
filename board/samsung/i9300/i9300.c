/*
 * i9300 board file
 * Copyright (C) 2018 Simon Shields <simon@lineageos.org>
 *
 * SPDX-License-Identifier: GPL-2.0+
 */
#include <common.h>
#include <asm/gpio.h>
#include <asm/arch/gpio.h>
#include <extcon.h>
#include <linux/libfdt.h>
#include <power/max77686_pmic.h>
#include <power/max77693_muic.h>
#include <power/pmic.h>
#include <power/regulator.h>
#include <usb.h>
#include <usb/dwc2_udc.h>

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

static int i9300_phy_control(int on)
{
	int ret;
	int type;
	struct udevice *vuotg;
	struct udevice *safeout;
	struct udevice *extcon;
	struct udevice *pmic;

	ret = regulator_get_by_platname("VUOTG_3.0V", &vuotg);
	if (ret) {
		pr_err("Failed to get VUOTG_3.0V: %d\n", ret);
		return -1;
	}

	ret = regulator_get_by_platname("ESAFEOUT1", &safeout);
	if (ret) {
		pr_err("Failed to get ESAFEOUT1: %d\n", ret);
		return -1;
	}

	ret = extcon_get("muic-max77693", &extcon);
	if (ret) {
		pr_err("Failed to get max77693 extcon: %d\n", ret);
		return -1;
	}

	ret = pmic_get("max77693_muic@25", &pmic);
	if (ret) {
		pr_err("Failed to get MUIC PMIC: %d!\n", ret);
		return -1;
	}

	pr_info("Waiting 10 seconds for USB to be inserted...\n");
	int i = 0;
	do {
		extcon_get_cable_id(extcon, &type);
		if (type & EXTCON_TYPE_USB)
			break;
		mdelay(1000);
	} while (i++ < 10);

	if (!(type & EXTCON_TYPE_USB)) {
		pr_info("No USB cable detected, aborting!\n");
		return -1;
	}

	if (on) {
		ret = regulator_set_mode(vuotg, OPMODE_ON);
		if (ret) {
			pr_err("Failed to set VUOTG_3.0V to ON: %d\n", ret);
			return -1;
		}

		ret = regulator_set_enable(safeout, true);
		if (ret) {
			pr_err("Failed to enable ESAFEOUT1: %d\n", ret);
			return -1;
		}

		/* set MUIC path to USB */
		ret = pmic_reg_write(pmic, MAX77693_MUIC_CONTROL1,
				MAX77693_MUIC_CTRL1_DN1DP2);
		if (ret) {
			pr_err("Failed to set MUIC path to USB: %d\n", ret);
			return -1;
		}
	} else {
		ret = regulator_set_mode(vuotg, OPMODE_LPM);
		if (ret) {
			pr_err("Failed to set VUOTG_3.0V to LPM: %d\n", ret);
			return -1;
		}

		/* set MUIC path to UART */
		ret = pmic_reg_write(pmic, MAX77693_MUIC_CONTROL1,
				MAX77693_MUIC_CTRL1_UT1UR2);
		if (ret) {
			pr_err("Failed to set MUIC path to UART: %d\n", ret);
			return -1;
		}

	}


	return 0;
}

struct dwc2_plat_otg_data exynos4_otg_data = {
	.phy_control = i9300_phy_control,
	.regs_phy = EXYNOS4X12_USBPHY_BASE,
	.regs_otg = EXYNOS4X12_USBOTG_BASE,
	.usb_phy_ctrl = EXYNOS4X12_USBPHY_CONTROL,
	.usb_flags = PHY0_SLEEP,
};

int board_usb_init(int index, enum usb_init_type init)
{
	pr_info("Board usb init! %d %d\n", index, init);
	return dwc2_udc_probe(&exynos4_otg_data);
}

#ifdef CONFIG_OF_BOARD_SETUP
int ft_board_setup(void *blob, bd_t *bd)
{
#if 0
	int ret;
	/* we don't want the OS to think we're running under secure firmware */
	int offs = fdt_node_offset_by_compatible(blob, 0, "samsung,secure-firmware");
	if (offs < 0) {
		if (offs == -FDT_ERR_NOTFOUND) {
			printf("%s: no secure firmware node!\n", __func__);
			return 0;
		}
		printf("%s: failed to find secure firmware node: %d\n", __func__, offs);
		return -EINVAL;
	}

	/* delete the node */
	ret = fdt_setprop_string(blob, offs, "test", "blah");
	if (ret < 0) {
		printf("%s: failed to remove secure firmware node: %d\n", __func__, ret);
		return -EINVAL;
	}

	printf("FDT set up for OS %p\n", blob);
#endif
	return 0;
}
#endif

int exynos_init(void)
{
	board_gpio_init();

	return 0;
}
