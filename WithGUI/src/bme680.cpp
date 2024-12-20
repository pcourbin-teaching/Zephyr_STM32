#include "bme680.hpp"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app_bme680);

void myBME680::init(const struct device *bme680_dev)
{
    dev = bme680_dev;
    if (!device_is_ready(dev))
    {
        LOG_ERR("Could not get %s device. Using false data.", dev->name);
    }
}

void myBME680::update_values()
{
    if (device_is_ready(dev))
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

double myBME680::get_temperature()
{
    return sensor_value_to_double(&temperature);
}

double myBME680::get_humidity()
{
    return sensor_value_to_double(&humidity);
}
