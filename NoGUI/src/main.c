/**
 ******************************************************************************
 * @file    main.c
 * @author  P. COURBIN
 * @version V2.0
 * @date    08-12-2023
 * @brief   NoGUI version
 ******************************************************************************
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

#define STACKSIZE (4096)
static K_THREAD_STACK_DEFINE(led0_stack, STACKSIZE);
static K_THREAD_STACK_DEFINE(led1_stack, STACKSIZE);
static K_THREAD_STACK_DEFINE(sw0_stack, STACKSIZE);

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define SW0_NODE DT_ALIAS(sw0)
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec sw0 = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0});
static struct gpio_callback sw0_cb_data;

K_MUTEX_DEFINE(mutexLEDs);
int switchPushed = 0;

void update_leds(uint8_t led0_val, uint8_t led1_val)
{
    if (k_mutex_lock(&mutexLEDs, K_FOREVER) == 0)
    {
        gpio_pin_set_dt(&led0, led0_val);
        gpio_pin_set_dt(&led1, led1_val);
        k_mutex_unlock(&mutexLEDs);
    }
}

static void led0_task(void *p1, void *p2, void *p3)
{
    int i, nbIter = 100000;
    while (1)
    {
        for (i = 0; i < nbIter; i++)
        {
            update_leds(1, 0);
        }
        update_leds(0, 0);
        k_msleep(1000);
    }
}

static void led1_task(void *p1, void *p2, void *p3)
{
    int i, nbIter = 200000;
    while (1)
    {
        for (i = 0; i < nbIter; i++)
        {
            update_leds(0, 1);
        }
        update_leds(0, 0);
        k_msleep(2000);
    }
}

static void sw0_task(void *p1, void *p2, void *p3)
{
    int i, nbIter = 500000;
    while (1)
    {
        if (switchPushed == 1)
        {
            switchPushed = 0;
            for (i = 0; i < nbIter; i++)
            {
                update_leds(1, 1);
            }
            update_leds(0, 0);
        }
        k_yield();
    }
}

void sw0_pressed_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    LOG_INF("Switch pressed at %d" PRIu32 "\n", k_cycle_get_32());
    if ((pins & (1 << sw0.pin)) != 0)
    {
        switchPushed = 1;
    }
}

uint8_t init_leds()
{
    uint8_t returned = 0;
    if (!device_is_ready(led0.port) || !device_is_ready(led1.port))
    {
        LOG_ERR("Error: LEDs devices are not ready (%s / %s)", led0.port->name, led1.port->name);
        returned = -1;
    }

    if (gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE) < 0 || gpio_pin_configure_dt(&led1, GPIO_OUTPUT_ACTIVE) < 0)
    {
        LOG_ERR("Error: LEDs config failed (%s / %s).", led0.port->name, led1.port->name);
        returned = -2;
    }
    return returned;
}

uint8_t init_switches()
{
    uint8_t returned = 0;

    if (!device_is_ready(sw0.port))
    {
        LOG_ERR("Error: Switch device %s is not ready.", sw0.port->name);
        returned = -1;
    }

    if (!returned && gpio_pin_configure_dt(&sw0, GPIO_INPUT) != 0)
    {
        LOG_ERR("Error: failed to configure switch %s pin %d.", sw0.port->name, sw0.pin);
        returned = -2;
    }

    if (!returned && gpio_pin_interrupt_configure_dt(&sw0, GPIO_INT_EDGE_TO_ACTIVE) != 0)
    {
        LOG_ERR("Error: failed to configure interrupt on %s pin %d.", sw0.port->name, sw0.pin);
        returned = -3;
    }

    if (!returned)
    {
        gpio_init_callback(&sw0_cb_data, sw0_pressed_callback, BIT(sw0.pin));
        gpio_add_callback(sw0.port, &sw0_cb_data);
    }

    LOG_INF("Set up switch at %s pin %d", sw0.port->name, sw0.pin);
    return returned;
}

void main(void)
{

    struct k_thread led0_t, led1_t, sw0_t;
    if (init_leds() < 0 || init_switches() < 0)
    {
        LOG_ERR("Error: %s", "LED or Switch init failed");
        return;
    }

    k_thread_create(&led0_t, led0_stack, K_THREAD_STACK_SIZEOF(led0_stack),
                    led0_task, NULL, NULL, NULL,
                    1, 0, K_NO_WAIT);
    k_thread_create(&led1_t, led1_stack, K_THREAD_STACK_SIZEOF(led1_stack),
                    led1_task, NULL, NULL, NULL,
                    2, 0, K_NO_WAIT);
    k_thread_create(&sw0_t, sw0_stack, K_THREAD_STACK_SIZEOF(sw0_stack),
                    sw0_task, NULL, NULL, NULL,
                    3, 0, K_NO_WAIT);
}