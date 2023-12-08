#ifndef BME680_HPP
#define BME680_HPP

#include <zephyr/drivers/sensor.h>

#define BME680_DEV DT_CHOSEN(perso_bme680) // OR DT_ALIAS(bme680)

class myBME680
{
public:
    const struct device *dev;

    struct sensor_value temperature;
    struct sensor_value humidity;
    int false_value = 0;

    void init();

    void update_values();
    double get_temperature();
    double get_humidity();
};

#endif