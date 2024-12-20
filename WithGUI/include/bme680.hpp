#ifndef BME680_HPP
#define BME680_HPP

#include <zephyr/drivers/sensor.h>

class myBME680
{
public:
    const struct device *dev;

    struct sensor_value temperature;
    struct sensor_value humidity;
    int false_value = 0;

    void init(const struct device *bme680_dev);

    void update_values();
    double get_temperature();
    double get_humidity();
};

#endif