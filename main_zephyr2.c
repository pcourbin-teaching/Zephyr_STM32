/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <kernel.h>
#include <drivers/gpio.h>


/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

/* The devicetree node identifier for the "led0" alias. */
// #define LED0_NODE DT_ALIAS(led0)
#define LED_GREEN_NODE DT_NODELABEL(green_led_4)
#define LED_ORANGE_NODE DT_NODELABEL(orange_led_3)
/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led_green = GPIO_DT_SPEC_GET(LED_GREEN_NODE, gpios);
static const struct gpio_dt_spec led_orange = GPIO_DT_SPEC_GET(LED_ORANGE_NODE, gpios);

void main(void)
{
	if (!device_is_ready(led_green.port) || !device_is_ready(led_orange.port)) {
		return;
	}

	if (gpio_pin_configure_dt(&led_green, GPIO_OUTPUT_ACTIVE) < 0 || gpio_pin_configure_dt(&led_orange, GPIO_OUTPUT_ACTIVE) < 0) {
		return;
	}
	gpio_pin_set_dt(&led_green, 0);
	gpio_pin_set_dt(&led_orange, 1);
	while (1) {
		if (gpio_pin_toggle_dt(&led_green) < 0 || gpio_pin_toggle_dt(&led_orange) < 0) {
			return;
		}
		k_msleep(SLEEP_TIME_MS);
	}
}