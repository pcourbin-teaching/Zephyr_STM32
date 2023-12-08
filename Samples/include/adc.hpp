#ifndef ADC_HPP
#define ADC_HPP

// See https://github.com/zephyrproject-rtos/zephyr/blob/zephyr-v3.5.0/samples/drivers/adc/

#include <zephyr/drivers/adc.h>
#define ADC_NODE DT_PHANDLE(DT_PATH(zephyr_user), io_channels)
#define ADC_CHANNEL DT_IO_CHANNELS_INPUT_BY_IDX(DT_PATH(zephyr_user), 0)

/* Common settings supported by most ADCs */
#define ADC_RESOLUTION 12
#define ADC_GAIN ADC_GAIN_1
#define ADC_REFERENCE ADC_REF_INTERNAL
#define ADC_ACQUISITION_TIME ADC_ACQ_TIME_DEFAULT

class myADC
{
public:
    const struct device *dev;

    uint8_t channel_id;
    int16_t sample_buffer[1];
    int32_t raw_value = -1;
    int32_t mv_value = -1;
    int32_t adc_vref;

    struct adc_channel_cfg channel_cfg;
    struct adc_sequence sequence;

    void init();

    void update_value();
    int32_t get_value();
    int32_t get_value_mv();
};

#endif