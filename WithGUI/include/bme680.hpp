#ifndef BME680_HPP
#define BME680_HPP

#include <drivers/sensor.h>

#define BME680_DEV_NAME DT_LABEL(DT_INST(0, bosch_bme680))

class BME680
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