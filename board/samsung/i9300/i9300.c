/*
 * i9300 board file
 * Copyright (C) 2018 Simon Shields <simon@lineageos.org>
 *
 * SPDX-License-Identifier: GPL-2.0+
 */
#include <common.h>
#include <asm/gpio.h>
#include <asm/arch/gpio.h>
#include <asm/arch/power.h>
#include <console.h>
#include <dm/uclass.h>
#include <extcon.h>
#include <led.h>
#include <linux/libfdt.h>
#include <mmc.h>
#include <power/max77686_pmic.h>
#include <power/max77693_muic.h>
#include <power/battery.h>
#include <power/charger.h>
#include <power/pmic.h>
#include <power/regulator.h>
#include <usb.h>
#include <usb/dwc2_udc.h>

#include "i9300.h"

DECLARE_GLOBAL_DATA_PTR;

#define KEY_POWER (0)
#define KEY_VOL_UP (1)
#define KEY_VOL_DOWN (2)
#define KEY_HOME (3)

#define COMBO_POWER (1 << KEY_POWER)
#define COMBO_VOL_UP (1 << KEY_VOL_UP)
#define COMBO_VOL_DOWN (1 << KEY_VOL_DOWN)
#define COMBO_HOME (1 << KEY_HOME)

#define COMBO_RECOVERY (COMBO_POWER | COMBO_VOL_UP | COMBO_HOME)
#define COMBO_FASTBOOT (COMBO_POWER | COMBO_VOL_DOWN | COMBO_HOME)
#define COMBO_UBOOT_CONSOLE (COMBO_POWER | COMBO_VOL_UP | COMBO_VOL_DOWN)

static uint64_t board_serial = 0;
static char board_rev = 0xff;
static char board_serial_str[17];

void board_load_info(void)
{
	struct mmc *emmc = find_mmc_device(0);
	static const int rev_gpios[] = {
		EXYNOS4X12_GPIO_M15,
		EXYNOS4X12_GPIO_M14,
		EXYNOS4X12_GPIO_M13,
		EXYNOS4X12_GPIO_M12,
	};

	board_rev = 0;

	for (int i = 0; i < ARRAY_SIZE(rev_gpios); i++) {
		gpio_request(rev_gpios[i], "HW_REV[0..3]");
		gpio_cfg_pin(rev_gpios[i], S5P_GPIO_INPUT);
		gpio_set_pull(rev_gpios[i], S5P_GPIO_PULL_DOWN);

		board_rev <<= 1;
		board_rev |= gpio_get_value(rev_gpios[i]);
	}

	if (!emmc) {
		pr_err("%s: couldn't get serial number - no eMMC device found!\n", __func__);
		sprintf(board_serial_str, "%16x", 0);
		return;
	}

	if (mmc_init(emmc)) {
		pr_err("%s: eMMC init failed!\n", __func__);
	} else {
		board_serial = ((uint64_t)emmc->cid[2] << 32) | emmc->cid[3];
	}
	sprintf(board_serial_str, "%16llx", board_serial);
	env_set("serial#", board_serial_str);
}

int get_board_rev(void) {
	if (board_rev == 0xff)
		board_load_info();

	return board_rev;
}

static void board_gpio_init(void)
{
	/* power and volume keys are externally pulled up */
	/*
	 * GPX2[7] - power key. If we don't set pull to none within 8 seconds,
	 * PMIC thinks power key is being held down and will reset the board.
	 */
	gpio_request(EXYNOS4X12_GPIO_X27, "nPOWER");
	gpio_cfg_pin(EXYNOS4X12_GPIO_X27, S5P_GPIO_INPUT);
	gpio_set_pull(EXYNOS4X12_GPIO_X27, S5P_GPIO_PULL_NONE);

	/* GPX2[2] - volume up */
	gpio_request(EXYNOS4X12_GPIO_X22, "VOL_UP");
	gpio_cfg_pin(EXYNOS4X12_GPIO_X22, S5P_GPIO_INPUT);
	gpio_set_pull(EXYNOS4X12_GPIO_X22, S5P_GPIO_PULL_NONE);

	/* GPX3[3] - volume down */
	gpio_request(EXYNOS4X12_GPIO_X33, "VOL_DOWN");
	gpio_cfg_pin(EXYNOS4X12_GPIO_X33, S5P_GPIO_INPUT);
	gpio_set_pull(EXYNOS4X12_GPIO_X33, S5P_GPIO_PULL_NONE);

	/* GPX0[1] - home key */
	gpio_request(EXYNOS4X12_GPIO_X01, "HOME");
	gpio_cfg_pin(EXYNOS4X12_GPIO_X01, S5P_GPIO_INPUT);
	gpio_set_pull(EXYNOS4X12_GPIO_X01, S5P_GPIO_PULL_NONE);
}

static int i9300_check_battery(void)
{
	struct udevice *bat, *extcon, *charger;
	int ret, state, current, old_soc, soc;

	ret = uclass_get_device(UCLASS_BATTERY, 0, &bat);
	if (ret) {
		printf("%s: failed to get battery device: %d\n", __func__, ret);
		return ret;
	}

	ret = uclass_get_device(UCLASS_EXTCON, 0, &extcon);
	if (ret) {
		printf("%s: failed to get extcon device: %d\n", __func__, ret);
		return ret;
	}

	current = extcon_get_max_charge_current(extcon);
	if (current < 0) {
		printf("%s: Failed to get max charge current: %d\n", __func__, current);
		return current;
	}

	old_soc = battery_get_soc(bat);
	state = battery_get_status(bat);
	if (state != BAT_STATE_NEED_CHARGING) {
		printf("%s: Battery soc is OK - %d\n", __func__, old_soc);
		return current > 0 ? BATTERY_LPM : BATTERY_NORMAL;
	}

	ret = uclass_get_device(UCLASS_CHARGER, 0, &charger);
	if (ret) {
		printf("%s: failed to get charger device: %d\n", __func__, ret);
		return ret;
	}

	if (current == 0)
		current = 460 * 1000;

	ret = charger_set_current(charger, current);
	if (ret < 0) {
		printf("%s: Failed to set charge current: %d\n", __func__, ret);
		return ret;
	}

	printf("Need charging: current charge level %d%, will charge at %d uA\n", battery_get_soc(bat), current);
	for (int i = 0; i < 50; i++) {
		i9300_led_action(LED_RED | LED_GREEN | LED_BLUE, LEDST_TOGGLE);
		mdelay(500);
		if (charger_get_status(charger) != CHARGE_STATE_DISCHARGING)
			break;
	}
	i9300_led_action(LED_RED | LED_GREEN | LED_BLUE, LEDST_OFF);
	if (charger_get_status(charger) == CHARGE_STATE_DISCHARGING ||
			charger_get_status(charger) == CHARGE_STATE_UNKNOWN) {
		printf("error: not charging. shutting down.");
		return BATTERY_ABORT;
	}

	printf("charging!\n");
	while (battery_get_status(bat) == BAT_STATE_NEED_CHARGING) {
		soc = battery_get_soc(bat);
		printf("%s: SoC started at %d, now %d\n", __func__, old_soc, soc);
		i9300_led_action(LED_RED, LEDST_TOGGLE);
		mdelay(500);
	}
	return BATTERY_LPM;
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

	if (on) {
		pr_info("Waiting 10 seconds for USB to be inserted...\n");
		int i = 0;
		do {
			extcon_get_cable_id(extcon, &type);
			if (type & EXTCON_TYPE_USB)
				break;
			if (ctrlc())
				break;
			mdelay(10);
		} while (i++ < 10000);

		if (!(type & EXTCON_TYPE_USB)) {
			pr_info("No USB cable detected, aborting!\n");
			return -1;
		}
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

int fb_set_reboot_flag(void)
{
	struct exynos4412_power *pwr = (struct exynos4412_power *)samsung_get_base_power();

	writel(INFORM_MAGIC | MODE_FASTBOOT, &pwr->inform3);
	return 0;
}

int board_usb_init(int index, enum usb_init_type init)
{
	pr_info("Board usb init! %d %d\n", index, init);
	return dwc2_udc_probe(&exynos4_otg_data);
}

int board_usb_cleanup(int index, enum usb_init_type init)
{
	return i9300_phy_control(0);
}

#ifdef CONFIG_OF_BOARD_SETUP
int ft_board_setup(void *blob, bd_t *bd)
{
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
	ret = fdt_del_node(blob, offs);
	if (ret < 0) {
		printf("%s: failed to remove secure firmware node: %d\n", __func__, ret);
		return -EINVAL;
	}

	printf("FDT set up for OS %p\n", blob);
	return 0;
}
#endif

static enum boot_mode i9300_check_keycombo(void)
{
	int ret = 0;

	ret |= !gpio_get_value(EXYNOS4X12_GPIO_X27) << KEY_POWER;
	ret |= !gpio_get_value(EXYNOS4X12_GPIO_X22) << KEY_VOL_UP;
	ret |= !gpio_get_value(EXYNOS4X12_GPIO_X33) << KEY_VOL_DOWN;
	ret |= !gpio_get_value(EXYNOS4X12_GPIO_X01) << KEY_HOME;

	switch (ret) {
	case COMBO_RECOVERY:
		return MODE_RECOVERY;
	case COMBO_FASTBOOT:
		return MODE_FASTBOOT;
	case COMBO_UBOOT_CONSOLE:
		return MODE_CONSOLE;
	}

	/* Power key pressed means that LPM should be skipped.
	 * TODO: there's probably a better way to determine wakeup source... */
	if (ret & KEY_POWER)
		return MODE_SKIP_LPM;

	return MODE_NONE;
}

static enum boot_mode i9300_get_boot_mode(void)
{
	struct exynos4412_power *pwr = (struct exynos4412_power *)samsung_get_base_power();

	u32 inform = readl(&pwr->inform3);
	/* clear out inform3 for subsequent boots */
	writel(0, &pwr->inform3);
	printf("inform3: 0x%08x: ", inform);
	if ((inform & ~MODE_MAX) != INFORM_MAGIC) {
		printf("invalid\n");
		return MODE_NONE;
	}

	inform &= MODE_MAX;

	printf("boot mode: %#x\n", inform);
	if (inform >= MODE_LAST)
		return MODE_NONE;

	return inform;
}

static void i9300_power_off(void)
{
	struct exynos4412_power *pwr = (struct exynos4412_power *)samsung_get_base_power();

	writel(readl(&pwr->ps_hold_control) & 0xfffffeff, &pwr->ps_hold_control);

	while (1) {
		i9300_led_action(LED_RED | LED_GREEN, LEDST_TOGGLE);
		mdelay(400);
	}
}

int exynos_init(void)
{
	board_gpio_init();

	printf("Key combo: %#x\n", i9300_check_keycombo());
	return 0;
}

int exynos_late_init(void)
{
	board_load_info();

	env_set("bootmode", "normal");
	enum boot_mode mode = i9300_get_boot_mode();
	if (mode == MODE_NONE)
		mode = i9300_check_keycombo();

	enum battery_boot_mode bat_state = i9300_check_battery();
	if (bat_state == BATTERY_ABORT) {
		/* release PS_HOLD - turn off board */
		i9300_power_off();
	} else if (bat_state == BATTERY_LPM && mode == MODE_NONE) {
		env_set("bootmode", "lpm");
	}

	switch (mode) {
	case MODE_FASTBOOT:
		printf("Activating fastboot mode\n");
		i9300_led_action(LED_BLUE, LEDST_ON);
		env_set("bootcmd", "run fastboot");
		break;
	case MODE_RECOVERY:
		printf("Booting to recovery\n");
		i9300_led_action(LED_RED, LEDST_ON);
		env_set("bootcmd", "run recoveryboot");
		env_set("bootmode", "recovery");
		break;
	case MODE_CONSOLE:
		printf("Dropping into u-boot console\n");
		i9300_led_action(LED_GREEN, LEDST_ON);
		env_set("bootcmd", NULL);
		break;
	default:
		printf("Booting normally...\n");
		i9300_led_action(LED_GREEN | LED_RED, LEDST_ON);
		env_set("bootcmd", "run autoboot");

	}

	return 0;
}
