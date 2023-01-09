#include "bme680.hpp"

#include <logging/log.h>
LOG_MODULE_DECLARE(app);

void BME680::init(){
  dev = device_get_binding(BME680_DEV_NAME);
  if (dev == NULL) {
    LOG_ERR("Could not get %s device. Using false data.", BME680_DEV_NAME);
  }
}

double BME680::get_temperature(){
    double value = 0.0;
    struct sensor_value s_value;
    if (dev != NULL){
        sensor_sample_fetch(dev);
        sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &s_value);
        value = sensor_value_to_double(&s_value);
    } else {
        false_value++;
        value = 10 + false_value % 25;
    }
    return value;
}

double BME680::get_humidity(){
    double value = 0.0;
    struct sensor_value s_value;
    if (dev != NULL){
        sensor_sample_fetch(dev);
        sensor_channel_get(dev, SENSOR_CHAN_HUMIDITY, &s_value);
        value = sensor_value_to_double(&s_value);
    } else {
        false_value++;
        value = 10 + false_value % 80;
    }
    return value;
}
