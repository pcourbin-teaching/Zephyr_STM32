/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <kernel.h>
#include <device.h>
#include <drivers/gpio.h>

#include "display.h"
#include "main.h"

LOG_MODULE_REGISTER(app);

/* The devicetree node identifier for the "led0" alias. */
#define SW0_NODE DT_ALIAS(sw0)
static const struct gpio_dt_spec sw0 = GPIO_DT_SPEC_GET(SW0_NODE, gpios);

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

uint32_t count = 0U;
static struct gpio_callback sw0_cb_data;
extern lv_obj_t * btn;
extern lv_obj_t* count_label;



static void btn_event_cb(lv_obj_t * btn, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
        static uint8_t cnt = 0;	
        cnt++;

        /*Get the first child of the button which is the label and change its text*/
        lv_obj_t * label = lv_obj_get_child(btn, NULL);
        lv_label_set_text_fmt(label, "Button: %d", cnt);
    }
}

void sw0_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins){
	LOG_INF("Button pressed at %" PRIu32 "\n", k_cycle_get_32());
	++count;
	btn_event_cb(btn, LV_EVENT_CLICKED);
}

uint8_t init_buttons() {
	uint8_t returned = 0;
	int ret;
	if (!device_is_ready(sw0.port))
	{
		LOG_ERR("Error: button device %s is not ready\n",sw0.port->name);
		returned = -1;
	}

	ret = gpio_pin_configure_dt(&sw0, GPIO_INPUT);
	if (ret < 0)
	{
		LOG_ERR("Error %d: failed to configure %s pin %d\n", ret, sw0.port->name, sw0.pin);
		returned = -1;
	}

	ret = gpio_pin_interrupt_configure_dt(&sw0, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		LOG_ERR("Error %d: failed to configure interrupt on %s pin %d\n", ret, sw0.port->name, sw0.pin);
		returned = -1;
	}

	if (returned==0){
		gpio_init_callback(&sw0_cb_data, sw0_pressed, BIT(sw0.pin));
		gpio_add_callback(sw0.port, &sw0_cb_data);
		LOG_INF("Set up button at %s pin %d\n", sw0.port->name, sw0.pin);
	}
	return returned;
}

void main(void)
{

    char count_str[11] = {0};
	init_buttons();
	init_gui(&btn_event_cb);

	while (1) {
		if ((count % 100) == 0U) {
			sprintf(count_str, "%d", count/100U);
			lv_label_set_text(count_label, count_str);
			btn_event_cb(btn, LV_EVENT_CLICKED);
		}

		lv_task_handler();
		k_msleep(SLEEP_TIME_MS);
		++count;
	}
}