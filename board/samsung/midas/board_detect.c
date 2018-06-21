#include <common.h>
#include <asm/arch/cpu.h>
#include <asm/gpio.h>
#include <dm/uclass.h>
#include <i2c.h>

enum board {
	BOARD_UNKNOWN = 0,
	BOARD_I9300 = 1,
	BOARD_I9305 = 2,
	BOARD_N7100 = 3,
	BOARD_N7105 = 4,
	BOARD_MAX = 5,
};

static enum board cur_board = BOARD_MAX;

static const char *board_compat[BOARD_MAX] = {
	[BOARD_UNKNOWN] = "samsung,midas",
	[BOARD_I9300] = "samsung,i9300",
	[BOARD_I9305] = "samsung,i9305",
	[BOARD_N7100] = "samsung,n7100",
	[BOARD_N7105] = "samsung,n7105",
};

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

	if (gpio_get_value(EXYNOS4X12_GPIO_Y10) &&
			gpio_get_value(EXYNOS4X12_GPIO_Y11))
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

	if (gpio_get_value(EXYNOS4X12_GPIO_Y23))
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

	if (!gpio_get_value(EXYNOS4X12_GPIO_A06) &&
			!gpio_get_value(EXYNOS4X12_GPIO_A07))
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

	if (i9300) {
		return BOARD_I9300;
	} else if (i9305) {
		return BOARD_I9305;
	} else if (n7100) {
		return BOARD_N7100;
	}

	return BOARD_N7105;
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


