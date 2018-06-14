
#define RED 0
#define GREEN 1
#define BLUE 2

#define LED_RED (1 << RED)
#define LED_GREEN (1 << GREEN)
#define LED_BLUE (1 << BLUE)

int i9300_enable_leds(int mask);
