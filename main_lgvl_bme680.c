/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <kernel.h>
#include <device.h>
#include <drivers/display.h>
#include <drivers/gpio.h>

#include <drivers/sensor.h>
#include <stdio.h>

#include <lvgl.h>
#include <stdio.h>
#include <string.h>


#define DISPLAY_DEV_NAME DT_LABEL(DT_INST(0, ilitek_ili9340))

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(app);


/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000


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


/**
 * Create a button with a label and react on Click event.
 */
lv_obj_t * btn = NULL;
void lv_ex_get_started_1(void)
{
    btn = lv_btn_create(lv_scr_act(), NULL);     /*Add a button the current screen*/
    lv_obj_set_pos(btn, 10, 10);                            /*Set its position*/
    lv_obj_set_size(btn, 120, 50);                          /*Set its size*/
    lv_obj_set_event_cb(btn, btn_event_cb);                 /*Assign a callback to the button*/

    lv_obj_t * label = lv_label_create(btn, NULL);          /*Add a label to the button*/
    lv_label_set_text(label, "Button");                     /*Set the labels text*/
}

void main(void)
{
	uint32_t count = 0U;
	char count_str[11] = {0};
	const struct device *display_dev;
	lv_obj_t *hello_world_label;
	lv_obj_t *count_label;

	display_dev = device_get_binding(DISPLAY_DEV_NAME);
	if (display_dev == NULL) {
		LOG_ERR("device not found. Aborting test.");
		return;
	}

	hello_world_label = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_text(hello_world_label, "Hello world!");
	lv_obj_align(hello_world_label, NULL, LV_ALIGN_CENTER, 0, 0);

	count_label = lv_label_create(lv_scr_act(), NULL);
	lv_obj_align(count_label, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

	lv_ex_get_started_1();
	display_blanking_off(display_dev);

	// BME680
	const struct device *dev_bme680 = device_get_binding(DT_LABEL(DT_INST(0, bosch_bme680)));
    // OR const struct device *dev_bme680 = device_get_binding(DT_LABEL(DT_ALIAS(bme680)));
	struct sensor_value temp, press, humidity, gas_res;

	printf("Device %p name is %s\n", dev_bme680, dev_bme680->name);



	while (1) {
		if ((count % 100) == 0U) {
			sprintf(count_str, "%d", count/100U);
			lv_label_set_text(count_label, count_str);
			btn_event_cb(btn, LV_EVENT_CLICKED);
		}

		// BME680
		sensor_sample_fetch(dev_bme680);
		sensor_channel_get(dev_bme680, SENSOR_CHAN_AMBIENT_TEMP, &temp);
		sensor_channel_get(dev_bme680, SENSOR_CHAN_PRESS, &press);
		sensor_channel_get(dev_bme680, SENSOR_CHAN_HUMIDITY, &humidity);
		sensor_channel_get(dev_bme680, SENSOR_CHAN_GAS_RES, &gas_res);
		printf("T: %d.%06d; P: %d.%06d; H: %d.%06d; G: %d.%06d\n",
				temp.val1, temp.val2, press.val1, press.val2,
				humidity.val1, humidity.val2, gas_res.val1,
				gas_res.val2);

		lv_label_set_text_fmt(hello_world_label, "Hello world!: %d.%2d", temp.val1, temp.val2);

		lv_task_handler();
		k_msleep(SLEEP_TIME_MS);
		++count;
	}
}