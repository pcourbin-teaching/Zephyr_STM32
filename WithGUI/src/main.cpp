/**
  ******************************************************************************
  * @file    main.c
  * @author  P. COURBIN
  * @version V2.0
  * @date    08-12-2023
  * @brief   WithGUI version
  ******************************************************************************
*/

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
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

const struct device *bme680_dev = DEVICE_DT_GET(DT_CHOSEN(perso_bme680)); // OR DT_ALIAS(bme680)
const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

myDisplay display;
myBME680 bme680;

#define TASK_PREPARE(given_period)                    \
    k_tid_t tid = k_current_get();                    \
    const char *name = k_thread_name_get(tid);        \
    int period = given_period;                        \
    uint32_t start;                                   \
    struct k_timer timer;                             \
    k_timer_init(&timer, NULL, NULL);                 \
    k_timer_start(&timer, K_MSEC(0), K_MSEC(period)); \
    LOG_DBG("Run task %s - Priority %d - Period %d\n", name, k_thread_priority_get(tid), period);

#define TASK_START()                \
    k_timer_status_sync(&timer);    \
    LOG_DBG("START task %s", name); \
    start = k_uptime_get_32();

#define TASK_END() \
    LOG_DBG("END task %s - %dms", name, k_uptime_get_32() - start);


static void display_task(void *p1, void *p2, void *p3)
{
    TASK_PREPARE(PERIOD_DISPLAY_TASK);
    char text[50] = {0};

    while (1)
    {
        TASK_START()

        display.task_handler();
        display.chart_add_temperature(bme680.get_temperature());
        display.chart_add_humidity(bme680.get_humidity());

        sprintf(text, "%d.%02d - %d.%02d", bme680.temperature.val1, bme680.temperature.val2 / 10000, bme680.humidity.val1, bme680.humidity.val2 / 10000);
        display.text_add(text);

        TASK_END()
    }
}

static void bme680_task(void *p1, void *p2, void *p3)
{
    TASK_PREPARE(PERIOD_BME680_TASK);

    while (1)
    {
        TASK_START()
        
        bme680.update_values();
        LOG_INF("Temperature: %d.%06d °C, Humidity: %d.%06d %%", 
                bme680.temperature.val1, bme680.temperature.val2 / 10000,
                bme680.humidity.val1, bme680.humidity.val2 / 10000);

        TASK_END()
    }
}

int main(void)
{
	struct k_thread display_t;
    k_tid_t display_tid;
	struct k_thread bme680_t;
    k_tid_t bme680_tid;

	display.init(display_dev, true);
	bme680.init(bme680_dev);
	
	display_tid = k_thread_create(&display_t, display_stack, K_THREAD_STACK_SIZEOF(display_stack),
					display_task, NULL, NULL, NULL,
					PRIO_DISPLAY_TASK, 0, K_NO_WAIT);
    k_thread_name_set(display_tid, "display");
    
	bme680_tid = k_thread_create(&bme680_t, bme680_stack, K_THREAD_STACK_SIZEOF(bme680_stack),
					bme680_task, NULL, NULL, NULL,
					PRIO_BME680_TASK, 0, K_NO_WAIT);
    k_thread_name_set(bme680_tid, "bme680");
}