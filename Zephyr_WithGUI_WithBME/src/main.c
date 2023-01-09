/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <kernel.h>
#include <device.h>
#include <drivers/display.h>
#include <drivers/sensor.h>
#include <drivers/gpio.h>

#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



#define DISPLAY_DEV_NAME DT_LABEL(DT_INST(0, ilitek_ili9340))

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(app);


/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   100

#include "ESIEALogo.h"

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
    lv_obj_set_pos(btn, 60, 100);                            /*Set its position*/
    lv_obj_set_size(btn, 120, 50);                          /*Set its size*/
    lv_obj_set_event_cb(btn, btn_event_cb);                 /*Assign a callback to the button*/

    lv_obj_t * label = lv_label_create(btn, NULL);          /*Add a label to the button*/
    lv_label_set_text(label, "Button");                     /*Set the labels text*/
}

lv_chart_series_t * serie_temperature = NULL;
lv_chart_series_t * serie_humidity = NULL;
lv_obj_t * chart = NULL;
void lv_create_chart(){
	/*Create a chart*/
    
    chart = lv_chart_create(lv_scr_act(), NULL);
    lv_obj_set_size(chart, 200, 150);
    lv_obj_align(chart, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);   /*Show lines and points too*/
	lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);
	lv_chart_set_point_count(chart, 100);

	/*Add a faded are effect*/
    lv_obj_set_style_local_bg_opa(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_OPA_50); /*Max. opa.*/
    lv_obj_set_style_local_bg_grad_dir(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_obj_set_style_local_bg_main_stop(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 255);    /*Max opa on the top*/
    lv_obj_set_style_local_bg_grad_stop(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 0);      /*Transparent on the bottom*/


	lv_chart_set_y_range(chart, LV_CHART_AXIS_PRIMARY_Y, 20, 35);
	lv_chart_set_y_range(chart, LV_CHART_AXIS_SECONDARY_Y, 0, 100);
	//lv_chart_set_x_tick_length(chart, 10, LV_CHART_TICK_LENGTH_AUTO);
    /*Add two data series*/
    serie_temperature = lv_chart_add_series(chart, LV_COLOR_GREEN);
    serie_humidity = lv_chart_add_series(chart, LV_COLOR_BLUE);
	lv_chart_set_series_axis(chart, serie_humidity, LV_CHART_AXIS_SECONDARY_Y);
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
	display_set_orientation(display_dev, DISPLAY_ORIENTATION_ROTATED_180);

	/*hello_world_label = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_text(hello_world_label, "Hello world!");
	lv_obj_align(hello_world_label, NULL, LV_LABEL_ALIGN_AUTO, 0, 0);*/

	count_label = lv_label_create(lv_scr_act(), NULL);
	lv_obj_align(count_label, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
	lv_label_set_align(count_label, LV_LABEL_ALIGN_CENTER);

	

	/*Image From variable*/
	lv_obj_t * icon = lv_img_create(lv_scr_act(), NULL);
	LV_IMG_DECLARE(ESIEALogo);
	lv_img_set_src(icon, &ESIEALogo);
	lv_obj_align(icon, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
	
	lv_create_chart();
	//lv_ex_get_started_1();
	display_blanking_off(display_dev);

	const struct device *dev_bme680 = device_get_binding(DT_LABEL(DT_INST(0, bosch_bme680)));
	struct sensor_value temp, humidity;
	double temp_value = 0.0;
	double humidity_value = 0.0;

	printf("%p\n", dev_bme680);

	while (1) {
		/*if ((count % 100) == 0U) {
			sprintf(count_str, "%d", count/100U);
			lv_label_set_text(count_label, count_str);
			//btn_event_cb(btn, LV_EVENT_CLICKED);
		}*/
		lv_task_handler();
		k_msleep(SLEEP_TIME_MS);
		++count;
		if (dev_bme680 != NULL){
			sensor_sample_fetch(dev_bme680);
			sensor_channel_get(dev_bme680, SENSOR_CHAN_AMBIENT_TEMP, &temp);
			sensor_channel_get(dev_bme680, SENSOR_CHAN_HUMIDITY, &humidity);
			temp_value = sensor_value_to_double(&temp);
			humidity_value = sensor_value_to_double(&humidity);
			lv_chart_set_next(chart, serie_temperature, temp_value);
			lv_chart_set_next(chart, serie_humidity, humidity_value);

			sprintf(count_str, "%d.%02d - %d.%02d", temp.val1, temp.val2/1000, humidity.val1, humidity.val2/1000);
			lv_label_set_text(count_label, count_str);
			
		} else {
			temp_value = 10+count%25;
			lv_chart_set_next(chart, serie_temperature, temp_value);
			sprintf(count_str, "%d", (int)temp_value);
			lv_label_set_text(count_label, count_str);
		}
	}
}