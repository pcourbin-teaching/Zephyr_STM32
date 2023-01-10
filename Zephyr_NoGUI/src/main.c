/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <kernel.h>
#include <drivers/gpio.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(app);

#define STACKSIZE (4096)
static K_THREAD_STACK_DEFINE(thread0_stack, STACKSIZE);
static K_THREAD_STACK_DEFINE(thread1_stack, STACKSIZE);

int buttonPushed = 0;

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define SW0_NODE DT_ALIAS(sw0)
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec sw0 = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0});

static void generic_task_entry(void *p1, void *p2, void *p3)
{
	uint16_t period = ((task *)p1)->period;
	uint16_t cpu = ((task *)p1)->cpu;
	uint8_t led0 = ((task *)p1)->led0;
	uint8_t led1 = ((task *)p1)->led1;

	struct k_timer timer;
	k_timer_init(&timer, NULL, NULL);
	k_timer_start(&timer, K_MSEC(0), K_MSEC(period));
	LOG_INF("Run task %p - Priority %d\n", ((task *)p1)->tid, ((task *)p1)->priority);
	while (1)
	{
		k_timer_status_sync(&timer);
		LOG_INF("START task %p\n", ((task *)p1)->tid);
		burnCPU(cpu, led0, led1);
		LOG_INF("END task %p\n", ((task *)p1)->tid);
		update_leds(0, 0);
	}
}

uint8_t init_leds()
{
	uint8_t returned = 0;
	if (!device_is_ready(led0.port) || !device_is_ready(led1.port))
	{
		returned = -1;
	}

	if (gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE) < 0 || gpio_pin_configure_dt(&led1, GPIO_OUTPUT_ACTIVE) < 0)
	{
		returned = -1;
	}
	return returned;
}

void main(void)
{

	uint8_t i, nb_task = sizeof(tasks) / sizeof(tasks[0]);

	if (init_leds() < 0)
	{
		LOG_ERR("Error: %s", "LED init failed");
		return;
	}

	calibrateBurnCPU();
	for (i = 0; i < nb_task; i++)
	{
		LOG_INF("Prepare task %d\n\tstart %d\n\tperiod: %d\n\tcpu: %d\n\tpriority: %d\n", i, tasks[i].start, tasks[i].period, tasks[i].cpu, tasks[i].priority);
		tasks[i].tid = k_thread_create(&tasks[i].thread_p, tasks[i].thread_stack, STACKSIZE,
									   generic_task_entry, (void *)&tasks[i], NULL, NULL,
									   K_PRIO_PREEMPT(tasks[i].priority), 0,
									   K_MSEC(DELAY_START_TIME_MS + tasks[i].start));

		if (tasks[i].tid == NULL)
			return;
		k_thread_start(&tasks[i].thread_p);
	}
}