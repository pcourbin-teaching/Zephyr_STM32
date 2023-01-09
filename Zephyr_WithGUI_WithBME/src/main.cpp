/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <stdio.h>
#include <string.h>

#include "display.hpp"
#include "bme680.hpp"

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(app);

Display display;
BME680 bme680;

void main(void)
{
	char text[50] = {0};
	double temperature, humidity;

	display.init(true);
	bme680.init();

	while (1) {
		display.task_handler();
		temperature = bme680.get_temperature();
		display.chart_add_temperature(temperature);

		humidity = bme680.get_humidity();
		display.chart_add_humidity(humidity);

		sprintf(text, "%d.%02d - %d.%02d", (int)temperature, (int)(temperature*100-((int)temperature)*100), (int)humidity, (int)(humidity*100-((int)humidity)*100));
		display.text_add(text);

		k_msleep(1000);
	}
}