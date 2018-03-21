/* This is for wndr3400v3's gpio definition only */

#include <config.h>
#include <common.h>
#include <asm/addrspace.h>
#include <asm/types.h>
#include "ar7240_soc.h"

/* GPIO pins */
#define GREEN_LED_GPIO	17  /* power green led */
#define AMBER_LED_GPIO	14  /* power amber led */
#define RESET_BUTTON_GPIO	12	/* reset button */

#define AR9344_GPIO_OE	0x18040000
#define	AR9344_GPIO_IN	0x18040004
#define	AR9344_GPIO_OUT	0x18040008
#define	AR9344_GPIO_FUNCTION	0x1804006c

/*
 * Set gpio to output enable
 * AR9344 Datasheet 8.3.1 GPIO Function, page 125
 * 0	The bit is used as output
 */
void ar9344_gpio_config_output(int gpio)
{
	ar7240_reg_rmw_clear(AR9344_GPIO_OE, (1 << gpio));
}

/*
 * Set val to gpio pin(output mode)
 * AR9344 Datasheet 8.3.1 GPIO Function, page 125
 * 1	The bit is used as input
 */
void ar9344_gpio_config_input(int gpio)
{
	ar7240_reg_rmw_set(AR9344_GPIO_OE, (1 << gpio));
}

void ar9344_gpio_out_val(int gpio, int val)
{
	if (val & 0x1) {
		ar7240_reg_rmw_set(AR9344_GPIO_OUT, (1 << gpio));
	} else {
		ar7240_reg_rmw_clear(AR9344_GPIO_OUT, (1 << gpio));
	}
}

/*
 * Get current values of gpio pin(input mode)
 * AR9344 Datasheet 8.3.1 GPIO Function, page 125
 */
int ar9344_gpio_in_val(int gpio)
{
	return ((1 << gpio) & (ar7240_reg_rd(AR9344_GPIO_IN)));
}

/* Disable JTAG port functionality to enable GPIO 0~3 functionality
 * AR9344 Datasheet 8.3.23 GPIO Function, page 131
 * GPIO Function, bit 1, Disable JTAG port functionality to enable GPIO
 * functionality
 */
void ar9344_disable_JTAG(void)
{
	ar7240_reg_rmw_set(AR9344_GPIO_FUNCTION, (1 << 2));
}

/*
 * Turn on green led
 */
void ar9344_power_led_on(void)
{
	ar9344_gpio_config_output(GREEN_LED_GPIO);
	ar9344_gpio_out_val(GREEN_LED_GPIO, 0);
}

/*
 * Turn on green led
 */
void ar9344_power_led_off(void)
{
	ar9344_gpio_config_output(GREEN_LED_GPIO);
	ar9344_gpio_out_val(GREEN_LED_GPIO, 1);
}

/*
 * Turn on amber led
 */
void ar9344_system_led_on(void)
{
	ar9344_gpio_config_output(AMBER_LED_GPIO);
	ar9344_gpio_out_val(AMBER_LED_GPIO, 0);
}

/*
 * Turn off amber led
 */
void ar9344_system_led_off(void)
{
	ar9344_gpio_config_output(AMBER_LED_GPIO);
	ar9344_gpio_out_val(AMBER_LED_GPIO, 1);
}

/* GPIO 12: RESET SWITCH (Input Pin/Active LOW) */
/* return value 0: not pressed, 1: pressed*/
int ar9344_reset_button_is_press(void)
{
	ar9344_gpio_config_input(RESET_BUTTON_GPIO);
	if(ar9344_gpio_in_val(RESET_BUTTON_GPIO) == 0)
		return 1;
	else
		return 0;
}

/*
 * Disable 4 Lan 100 Led(GPIO 18~21) & Wan 100 Led(GPIO 22)
 *
 */
void disable_eth_led(void)
{
	int gpio;
	for (gpio = 18; gpio <= 22; gpio++) {
		ar9344_gpio_config_output(gpio);
		ar9344_gpio_out_val(gpio, 1);
	}
}
