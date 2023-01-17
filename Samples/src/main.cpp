#include <zephyr.h>
#include <stdio.h>
#include <string.h>

#include "display.hpp"
#include "bme680.hpp"
#include "adc.hpp"

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(app);

myDisplay display;
myBME680 bme680;
myADC adc;

void main(void)
{
	char text[50] = {0};

	display.init(true);
	bme680.init();
	adc.init();

	while (1)
	{

		bme680.update_values();
		adc.update_value();

		display.task_handler();
		display.chart_add_temperature(bme680.get_temperature());
		display.chart_add_humidity(bme680.get_humidity());

		sprintf(text, "T:%d.%02d - H:%d.%02d - A:%04d", 
					bme680.temperature.val1, bme680.temperature.val2 / 10000, 
					bme680.humidity.val1, bme680.humidity.val2 / 10000, 
					adc.get_value());
		display.text_add(text);

		k_msleep(1000);
	}
}