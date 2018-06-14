
#define RED 0
#define GREEN 1
#define BLUE 2

#define LED_RED (1 << RED)
#define LED_GREEN (1 << GREEN)
#define LED_BLUE (1 << BLUE)

int i9300_led_action(int mask, enum led_state_t state);

#define INFORM_MAGIC 0x12345670
enum boot_mode {
	MODE_NONE = 0x0,
	MODE_FASTBOOT = 0x1,
	MODE_RECOVERY = 0x2,
	MODE_CONSOLE = 0x3,
	MODE_LAST = 0x4,

	MODE_MAX = 0xf,
};

enum battery_boot_mode {
	/* normal boot */
	BATTERY_NORMAL,
	/* abort boot - battery too low */
	BATTERY_ABORT,
	/* enter LPM */
	BATTERY_LPM,
};
