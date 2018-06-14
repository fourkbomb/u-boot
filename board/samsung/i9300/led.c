#include <common.h>
#include <dm/uclass.h>
#include <led.h>
#include "i9300.h"

DECLARE_GLOBAL_DATA_PTR;

static const char *leds[] = {
	[RED] = "red",
	[GREEN] = "green",
	[BLUE] = "blue",
};

static int i9300_set_led(const char *name, enum led_state_t state)
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

int i9300_enable_leds(int mask)
{
	int ret;
	for (int i = 0; i < ARRAY_SIZE(leds); i++) {
		if (mask & (1 << i)) {
			ret = i9300_set_led(leds[i], LEDST_ON);
			if (ret) {
				return ret;
			}
		}
	}

	return 0;
}
