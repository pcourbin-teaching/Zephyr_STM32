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

#include <lvgl.h>
#include <stdio.h>
#include <string.h>

#define CONFIG_LVGL_DISPLAY_DEV_NAME DT_LABEL(DT_INST(0, ilitek_ili9340))

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(app);


/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000



void main(void)
{
	uint32_t count = 0U;
	char count_str[11] = {0};
	const struct device *display_dev;
	lv_obj_t *hello_world_label;
	lv_obj_t *count_label;

	display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);
	if (display_dev == NULL) {
		LOG_ERR("device not found. Aborting test.");
		return;
	}

	hello_world_label = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_text(hello_world_label, "Hello world!");
	lv_obj_align(hello_world_label, NULL, LV_ALIGN_CENTER, 0, 0);

	count_label = lv_label_create(lv_scr_act(), NULL);
	lv_obj_align(count_label, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

	display_blanking_off(display_dev);

	while (1) {
		if ((count % 100) == 0U) {
			sprintf(count_str, "%d", count/100U);
			lv_label_set_text(count_label, count_str);
		}
		lv_task_handler();
		k_msleep(SLEEP_TIME_MS);
		++count;
	}
}