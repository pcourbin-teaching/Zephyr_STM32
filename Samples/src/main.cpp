/**
  ******************************************************************************
  * @file    main.c
  * @author  P. COURBIN
  * @version V2.0
  * @date    08-12-2023
  * @brief   Sample version
  ******************************************************************************
*/

#include <zephyr/kernel.h>
#include <stdio.h>
#include <string.h>

#include "display.hpp"
#include "bme680.hpp"
#include "adc.hpp"

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

myDisplay display;
myBME680 bme680;
myADC adc;

int main(void)
{
	char text[50] = {0};

	display.init(true);
	bme680.init();
	adc.init();

	lv_task_handler();
	display_blanking_off(display.dev);

	while (1)
	{
		bme680.update_values();
		adc.update_value();

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
}