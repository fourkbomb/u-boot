#include <common.h>
#include <dm/uclass.h>
#include <led.h>
#include "midas.h"

DECLARE_GLOBAL_DATA_PTR;

static const char *leds[] = {
	[RED] = "red",
	[GREEN] = "green",
	[BLUE] = "blue",
};

static int midas_set_led(const char *name, enum led_state_t state)
{
	struct udevice *dev;
	int ret;

	ret = led_get_by_label(name, &dev);
	if (ret) {
		printf("%s: get led %s failed: %d\n", __func__, name, ret);
		return ret;
	}

	return led_set_state(dev, state);
}

int midas_led_action(int mask, enum led_state_t state)
{
	int ret;
	for (int i = 0; i < ARRAY_SIZE(leds); i++) {
		if (mask & (1 << i)) {
			ret = midas_set_led(leds[i], state);
			if (ret) {
				return ret;
			}
		}
	}

	return 0;
}

