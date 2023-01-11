/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <logging/log.h>
LOG_MODULE_REGISTER(app);

#include "display.hpp"
#include "bme680.hpp"

#define STACKSIZE (4096)
static K_THREAD_STACK_DEFINE(display_stack, STACKSIZE);
static K_THREAD_STACK_DEFINE(bme680_stack, STACKSIZE);

#define PRIO_DISPLAY_TASK 1
#define PRIO_BME680_TASK 2
#define PERIOD_DISPLAY_TASK 1000
#define PERIOD_BME680_TASK 500

Display display;
BME680 bme680;

static void display_task(void *p1, void *p2, void *p3)
{
	char *name = log_strdup("DISPLAY");
	k_tid_t tid = k_current_get();
	int period = PERIOD_DISPLAY_TASK;

	char text[50] = {0};
	uint32_t start;

	struct k_timer timer;
	k_timer_init(&timer, NULL, NULL);
	k_timer_start(&timer, K_MSEC(0), K_MSEC(period));
	LOG_INF("Run task DISPLAY - Priority %d - Period %d\n", k_thread_priority_get(tid), period);
	while (1)
	{
		k_timer_status_sync(&timer);
		LOG_INF("START task %s", name);
		start = k_uptime_get_32();
		display.task_handler();
		display.chart_add_temperature(bme680.get_temperature());
		display.chart_add_humidity(bme680.get_humidity());

		sprintf(text, "%d.%02d - %d.%02d", bme680.temperature.val1, bme680.temperature.val2 / 10000, bme680.humidity.val1, bme680.humidity.val2 / 10000);
		display.text_add(text);
		LOG_INF("END task %s - %dms", name, k_uptime_get_32() - start);
	}
}

static void bme680_task(void *p1, void *p2, void *p3)
{
	char *name = log_strdup("BME680");
	k_tid_t tid = k_current_get();
	int period = PERIOD_BME680_TASK;

	uint32_t start;

	struct k_timer timer;
	k_timer_init(&timer, NULL, NULL);
	k_timer_start(&timer, K_MSEC(0), K_MSEC(period));
	LOG_INF("Run task BME680 - Priority %d - Period %d\n", k_thread_priority_get(tid), period);
	while (1)
	{
		k_timer_status_sync(&timer);
		LOG_INF("START task %s", name);
		start = k_uptime_get_32();
		bme680.update_values();
		LOG_INF("END task %s - %dms", name, k_uptime_get_32() - start);
	}
}

void main(void)
{
	struct k_thread display_t;
	struct k_thread bme680_t;

	display.init(true);
	bme680.init();

	k_thread_create(&display_t, display_stack, K_THREAD_STACK_SIZEOF(display_stack),
					display_task, NULL, NULL, NULL,
					PRIO_DISPLAY_TASK, 0, K_NO_WAIT);

	k_thread_create(&bme680_t, bme680_stack, K_THREAD_STACK_SIZEOF(bme680_stack),
					bme680_task, NULL, NULL, NULL,
					PRIO_BME680_TASK, 0, K_NO_WAIT);
}