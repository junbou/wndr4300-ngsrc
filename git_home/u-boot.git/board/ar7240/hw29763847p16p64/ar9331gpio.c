#include <config.h>
#include <common.h>
#include <asm/addrspace.h>
#include <asm/types.h>
#include "ar7240_soc.h"

/*
 * gpio configs
 */
#define AR9331_GPIO_BASE                        0x18040000
#define AR9331_GPIO_OE                          AR9331_GPIO_BASE+0x0
#define AR9331_GPIO_IN                          AR9331_GPIO_BASE+0x4
#define AR9331_GPIO_OUT                         AR9331_GPIO_BASE+0x8
#define AR9331_GPIO_SET                         AR9331_GPIO_BASE+0xc
#define AR9331_GPIO_CLEAR                       AR9331_GPIO_BASE+0x10
#define AR9331_GPIO_INT_ENABLE                  AR9331_GPIO_BASE+0x14
#define AR9331_GPIO_INT_TYPE                    AR9331_GPIO_BASE+0x18
#define AR9331_GPIO_INT_POLARITY                AR9331_GPIO_BASE+0x1c
#define AR9331_GPIO_INT_PENDING                 AR9331_GPIO_BASE+0x20
#define AR9331_GPIO_INT_MASK                    AR9331_GPIO_BASE+0x24
#define AR9331_GPIO_FUNCTIONS                   AR9331_GPIO_BASE+0x28
#define AR9331_GPIO_IN_ETH_SWITCH_LED           AR9331_GPIO_BASE+0x2c
#define AR9331_GPIO_FUNCTION_2                  AR9331_GPIO_BASE+0x30
#define AR9331_GPIO_FUNC_ZERO                   AR9331_GPIO_BASE+0x30

/* GPIO pins */
#define POWER_LED_GPIO 		POWER_LED	/* green */
#define TEST_LED_GPIO 	TEST_LED	/* amber */
#define RESET_BUTTON_GPIO	12	/* reset button */

/* LED active voltage */
/**
 * AR9331 GPIOs Define - WNR1000v4
 * GPIO 1: POWER LED (Output Pin/Active HIGH)
 * GPIO 27: SYSTEM  LED (Output Pin/Active LOW)
 **/
#define ACTIVE_HIGH_VOLTAGE     0x3
#define LED_ON(gpio)    (((1 << gpio) & ACTIVE_HIGH_VOLTAGE) ? 1 : 0)
#define LED_OFF(gpio)   (((1 << gpio) & ACTIVE_HIGH_VOLTAGE) ? 0 : 1)

void ar9331_gpio_config_output(int gpio)
{
	ar7240_reg_rmw_set(AR9331_GPIO_OE, (1 << gpio));
}

void ar9331_gpio_config_input(int gpio)
{
	ar7240_reg_rmw_clear(AR7240_GPIO_OE, (1 << gpio));
}

void ar9331_gpio_out_val(int gpio, int val)
{
	if (val & 0x1) {
		ar7240_reg_rmw_set(AR9331_GPIO_OUT, (1 << gpio));
	} else {
		ar7240_reg_rmw_clear(AR9331_GPIO_OUT, (1 << gpio));
	}
}

int ar9331_gpio_in_val(int gpio)
{
	return ((1 << gpio) & (ar7240_reg_rd(AR9331_GPIO_IN)));
}

/* Disable JTAG port functionality to enable GPIO functionality */
void ar9331_disable_JTAG(void)
{
	ar7240_reg_rmw_set(AR9331_GPIO_FUNCTIONS, (1 << 0));
}

/*
 * Set register 0x180600AC BIT[18]=1 to enables the MDC and MDIO function.
 * We could not control GPIO 26 & 27 without them.
 */
void enable_MDC_MDIO(void)
{
	ar7240_reg_rmw_set(0x180600AC, (1 << 18));
}

/*
 * Turn on green led
 */
void ar9331_power_led_on(void)
{
	ar9331_gpio_config_output(POWER_LED_GPIO);
	ar9331_gpio_out_val(POWER_LED_GPIO, 0);
}

/*
 * Turn off green led
 */
void ar9331_power_led_off(void)
{
	ar9331_gpio_config_output(POWER_LED_GPIO);
	ar9331_gpio_out_val(POWER_LED_GPIO, 1);
}

/*
 * Turn self test led to amber
 */
void ar9331_test_led_amber(void)
{
	ar9331_gpio_config_output(TEST_LED_GPIO);
	ar9331_gpio_config_output(22);

	ar7240_reg_wr(AR9331_GPIO_OUT, (ar7240_reg_rd(AR9331_GPIO_OUT)|BIT(21)) & ~(BIT(22)));
	ar7240_reg_rd(AR9331_GPIO_OUT);
}

/*
 * Turn self test led to green
 */
void ar9331_test_led_green(void)
{
	ar9331_gpio_config_output(TEST_LED_GPIO);
	ar9331_gpio_config_output(22);

	ar7240_reg_wr(AR9331_GPIO_OUT, (ar7240_reg_rd(AR9331_GPIO_OUT)|BIT(22)) & ~(BIT(21)));
	ar7240_reg_rd(AR9331_GPIO_OUT);
}

/*
 * Turn off self test led
 */
void ar9331_test_led_off(void)
{
	ar9331_gpio_config_output(TEST_LED_GPIO);
	ar9331_gpio_config_output(22);

	ar7240_reg_wr(AR9331_GPIO_OUT, (ar7240_reg_rd(AR9331_GPIO_OUT)|BIT(21)|BIT(22)));
	ar7240_reg_rd(AR9331_GPIO_OUT);
}

/* GPIO 12: RESET SWITCH (Input Pin/Active LOW) */
/* return value 0: not pressed, 1: pressed*/
int ar9331_reset_button_is_press()
{
	ar9331_gpio_config_input(RESET_BUTTON_GPIO);
	if(ar9331_gpio_in_val(RESET_BUTTON_GPIO) == 0)
		return 1;
	else
		return 0;
}
