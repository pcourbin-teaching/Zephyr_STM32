#include "bme680.hpp"

#include <logging/log.h>
LOG_MODULE_DECLARE(app);

void BME680::init()
{
    dev = device_get_binding(BME680_DEV_NAME);
    if (dev == NULL)
    {
        LOG_ERR("Could not get %s device. Using false data.", BME680_DEV_NAME);
    }
}

void BME680::update_values()
{
    if (dev != NULL)
    {
        sensor_sample_fetch(dev);
        sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temperature);
        sensor_channel_get(dev, SENSOR_CHAN_HUMIDITY, &humidity);
    }
    else
    {
        false_value++;
        sensor_value_from_double(&temperature, 10 + false_value % 25);
        sensor_value_from_double(&humidity, 10 + false_value % 80);
    }
}

double BME680::get_temperature()
{
    return sensor_value_to_double(&temperature);
}

double BME680::get_humidity()
{
    return sensor_value_to_double(&humidity);
}
