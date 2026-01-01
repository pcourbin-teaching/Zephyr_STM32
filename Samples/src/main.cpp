/**
  ******************************************************************************
  * @file    main.c
  * @author  P. COURBIN
  * @version V2.0
  * @date    19-12-2024
  * @brief   Sample version
  ******************************************************************************
*/

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <stdio.h>
#include <string.h>

#include "display.hpp"
#include "bme680.hpp"
#include "adc.hpp"
#include "servo.hpp"

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

const struct device *bme680_dev = DEVICE_DT_GET(DT_CHOSEN(perso_bme680)); // OR DT_ALIAS(bme680)
const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

static const struct pwm_dt_spec servo_dev = PWM_DT_SPEC_GET(DT_NODELABEL(servo));
static const uint32_t min_pulse = DT_PROP(DT_NODELABEL(servo), min_pulse);
static const uint32_t max_pulse = DT_PROP(DT_NODELABEL(servo), max_pulse);

myDisplay display;
myBME680 bme680;
myADC adc;
myServo servo;

int main(void)
{
	char text[50] = {0};

	display.init(display_dev,true);
	bme680.init(bme680_dev);
	adc.init();
	servo.init(&servo_dev, min_pulse, max_pulse);

	while (1)
	{
		bme680.update_values();
		adc.update_value();

		if (adc.get_value() > 2000){
			servo.setPosition(90);
            LOG_INF("Servo moving to 90 degrees");
		} else {
			servo.setPosition(0);
            LOG_INF("Servo moving to 0 degrees");
		}

		display.task_handler();
		display.chart_add_temperature(bme680.get_temperature());
		display.chart_add_humidity(bme680.get_humidity());

		sprintf(text, "T:%d.%02d°C \t H:%d.%02d\% \t A:%04d",
				bme680.temperature.val1, bme680.temperature.val2 / 10000,
				bme680.humidity.val1, bme680.humidity.val2 / 10000,
				adc.get_value());
		display.text_add(text);
		LOG_INF("%s\n", text);

		k_msleep(lv_task_handler());
	}

	return 0;
}