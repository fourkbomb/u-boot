#include <common.h>
#include <asm/arch/cpu.h>
#include <asm/gpio.h>
#include <dm/uclass.h>
#include <i2c.h>

/*
 * This code is run very early in U-Boot proper.
 * The DM GPIO stuff hasn't been initialised yet, so while
 * the gpio_cfg_pin/gpio_set_pull functions work (as they
 * are implemented directly by s5p_gpio), gpio_get_value
 * doesn't. So we need to use readl() instead.
 */
#define GPA0_DAT 0x11400004
#define GPY1_DAT 0x11000144
#define GPY2_DAT 0x11000164

extern void sdelay(unsigned long);

enum board {
	BOARD_UNKNOWN = 0,
	BOARD_I9300 = 1,
	BOARD_I9305 = 2,
	BOARD_N7100 = 3,
	BOARD_N7105 = 4,
	BOARD_MAX = 5,
};

static enum board cur_board = BOARD_MAX;

static const char *board_fit_name[BOARD_MAX] = {
	[BOARD_UNKNOWN] = "exynos4412-midas",
	[BOARD_I9300] = "exynos4412-i9300",
	[BOARD_I9305] = "exynos4412-i9305",
	[BOARD_N7100] = "exynos4412-n7100",
	[BOARD_N7105] = "exynos4412-n7105",
};

static bool is_i9305(void)
{
	/* On i9305:
	 * GPY1-0, GPY1-1: emulated I2C bus, have external pull-ups. When internally
	 * pulled down, value should stay high.
	 * On i9300/n7100/n7105:
	 * N/C - when internally pulled down, should go low.
	 */
	bool ret = false;
	for (int pin = EXYNOS4X12_GPIO_Y10; pin < EXYNOS4X12_GPIO_Y12; pin++) {
		gpio_cfg_pin(pin, S5P_GPIO_INPUT);
		gpio_set_pull(pin, S5P_GPIO_PULL_DOWN);
	}
	/* TRM says it takes 800 APB clocks for change to take effect */
	sdelay(0x8000);
	if ((readl(GPY1_DAT) & (BIT(0) | BIT(1))) == (BIT(0) | BIT(1)))
		ret = true;

	/* put everything back to reset value */
	for (int pin = EXYNOS4X12_GPIO_Y10; pin <= EXYNOS4X12_GPIO_Y12; pin++) {
		gpio_set_pull(pin, S5P_GPIO_PULL_UP);
	}

	return ret;
}

static bool is_i9300(void)
{
	/* On i9300:
	 * GPY2-3: emulated I2C bus SCL pin with external pull-up. When
	 * internally pulled down, value should stay high.
	 * On i9305/n7100/n7105:
	 * no external pullup. should go low when pulled up.
	 */
	bool ret = false;

	gpio_cfg_pin(EXYNOS4X12_GPIO_Y23, S5P_GPIO_INPUT);
	gpio_set_pull(EXYNOS4X12_GPIO_Y23, S5P_GPIO_PULL_DOWN);
	/* TRM says it takes 800 APB clocks for change to take effect */
	sdelay(0x8000);
	if ((readl(GPY2_DAT) & BIT(3)) == BIT(3))
		ret = true;

	gpio_set_pull(EXYNOS4X12_GPIO_Y23, S5P_GPIO_PULL_UP);

	return ret;
}

static bool is_n7100(void)
{
	/* On n7100/i9300:
	 * GPA0-6, GPA0-7: GPS UART flow control lines. Pull down means pulled down.
	 * On i9305/n7105:
	 * GPA0-6 GPA0-7: I2C bus with external pull ups. Pull down means still pulled up.
	 */
	bool ret = false;

	for (int pin = EXYNOS4X12_GPIO_A06; pin <= EXYNOS4X12_GPIO_A07; pin++) {
		gpio_cfg_pin(pin, S5P_GPIO_INPUT);
		gpio_set_pull(pin, S5P_GPIO_PULL_DOWN);
	}

	/* TRM says it takes 800 APB clocks for change to take effect */
	sdelay(0x8000);

	if ((readl(GPA0_DAT) & (BIT(6) | BIT(7))) == 0)
		ret = true;

	for (int pin = EXYNOS4X12_GPIO_A06; pin <= EXYNOS4X12_GPIO_A07; pin++) {
		gpio_set_pull(pin, S5P_GPIO_PULL_UP);
	}

	return ret;
}

enum board guess_board(void) {
	/* GPIO will fail if CPU ID hasn't been initialised yet */
	s5p_set_cpu_id();
	/* order is important here: we must check for i930x first,
	 * is_n7100() will return false positives on i9300
	 */
	bool i9300 = is_i9300();
	bool i9305 = is_i9305();
	bool n7100 = is_n7100();

	printf("%s: i9300-%d i9305-%d n7100-%d\n", __func__, i9300, i9305, n7100);
	if (i9300) {
		return BOARD_I9300;
	} else if (i9305) {
		return BOARD_I9305;
	} else if (n7100) {
		return BOARD_N7100;
	}

	return BOARD_N7105;
}

/*
 * This is called after relocation (for now),
 * so it should be fine to use the gpio_* APIs
 */
void init_overlays(void)
{
	enum board board = guess_board();
	env_set("fit_config", board_fit_name[board]);

	/* Note 2 has two panels - detect the right one. */
	if (board == BOARD_N7100 || board == BOARD_N7105) {
		gpio_request(EXYNOS4X12_GPIO_F10, "OLED_ID");
		gpio_cfg_pin(EXYNOS4X12_GPIO_F10, S5P_GPIO_INPUT);
		gpio_set_pull(EXYNOS4X12_GPIO_F10, S5P_GPIO_PULL_DOWN);

		if (gpio_get_value(EXYNOS4X12_GPIO_F10) == 0) {
			printf("%s: using EA8061\n", __func__);
			env_set("lcd_overlay", "#note2-ea8061");
		} else {
			printf("%s: using S6EVR02\n", __func__);
			env_set("lcd_overlay", "#note2-s6evr02");
		}
	}
}

#if defined(CONFIG_MULTI_DTB_FIT)
int board_fit_config_name_match(const char *name)
{
	if (cur_board == BOARD_MAX) {
		cur_board = guess_board();
	}

	if (!strcmp(name, board_fit_name[cur_board]))
		return 0;
	return -1;
}
#endif


