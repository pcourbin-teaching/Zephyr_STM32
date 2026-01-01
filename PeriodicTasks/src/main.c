/**
 ******************************************************************************
 * @file    main.c
 * @author  P. COURBIN
 * @version V2.0
 * @date    08-12-2023
 * @brief   PeriodicTasks version
 ******************************************************************************
 */

#include <zephyr/kernel.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

typedef struct task_t
{
	char name[20];
	struct k_thread thread_p;
	struct z_thread_stack_element *thread_stack;
	int start;
	int period;
	int cpu;
	int priority;
	uint8_t led0;
	uint8_t led1;
} task;

#define DELAY_START_TIME_MS 5000
#define STACKSIZE (1024)
static K_THREAD_STACK_DEFINE(thread0_stack, STACKSIZE);
static K_THREAD_STACK_DEFINE(thread1_stack, STACKSIZE);

task tasks[] = {
	{.name = "T1", .thread_stack = thread0_stack, .start = 0000, .period = 1000, .cpu = 400, .priority = 1, .led0 = 1, .led1 = 0},	// Bleu
	{.name = "T2", .thread_stack = thread1_stack, .start = 1000, .period = 3000, .cpu = 700, .priority = 2, .led0 = 0, .led1 = 1}}; // Jaune

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);

uint32_t cal = 1000;

void update_leds(uint8_t led0_val, uint8_t led1_val)
{
	gpio_pin_set_dt(&led0, led0_val);
	gpio_pin_set_dt(&led1, led1_val);
}

void burnCPU(uint16_t ms, uint8_t led0, uint8_t led1)
{
	while (ms--)
	{
		for (uint16_t i = 0; i < cal; i++)
		{
			update_leds(led0, led1);
			__asm("nop");
		}
	}
}

void calibrateBurnCPU()
{
	update_leds(1, 1);

	uint32_t objective_ms = 1000;
	uint32_t mesured_time = k_uptime_get();
	burnCPU(objective_ms, 1, 1);
	mesured_time = k_uptime_get() - mesured_time;
	cal = cal * objective_ms / mesured_time;
	update_leds(0, 0);
}

static void generic_task_entry(void *p1, void *p2, void *p3)
{
	char *name = ((task *)p1)->name;
	uint16_t start = ((task *)p1)->start;
	uint16_t period = ((task *)p1)->period;
	uint16_t cpu = ((task *)p1)->cpu;
	uint8_t led0 = ((task *)p1)->led0;
	uint8_t led1 = ((task *)p1)->led1;

	struct k_timer timer;
	k_timer_init(&timer, NULL, NULL);
	k_timer_start(&timer, K_MSEC(start), K_MSEC(period));
	LOG_INF("Run task %s - Priority %d", name, ((task *)p1)->priority);
	while (1)
	{
		k_timer_status_sync(&timer);
		LOG_INF("START task %s", name);
		burnCPU(cpu, led0, led1);
		LOG_INF("END task %s", name);
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

int main(void)
{

	uint8_t i, nb_task = sizeof(tasks) / sizeof(tasks[0]);

	if (init_leds() < 0)
	{
		LOG_ERR("Error: %s", "LED init failed");
		return -1;
	}

	calibrateBurnCPU();
	for (i = 0; i < nb_task; i++)
	{
		LOG_INF("Prepare task %d\n\tstart %d\n\tperiod: %d\n\tcpu: %d\n\tpriority: %d\n", i, tasks[i].start, tasks[i].period, tasks[i].cpu, tasks[i].priority);
		k_thread_create(&tasks[i].thread_p, tasks[i].thread_stack, STACKSIZE,
						generic_task_entry, (void *)&tasks[i], NULL, NULL,
						K_PRIO_PREEMPT(tasks[i].priority), 0,
						K_MSEC(DELAY_START_TIME_MS + tasks[i].start));
		k_thread_name_set(&tasks[i].thread_p, tasks[i].name);
	}

	return 0;
}