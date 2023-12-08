#include "adc.hpp"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(app);
void myADC::init()
{
    channel_id = ADC_CHANNEL;
    dev = DEVICE_DT_GET(ADC_NODE);
    if (!device_is_ready(dev))
    {
        dev = NULL;
        LOG_ERR("Could not init ADC on channel %d device.", channel_id);
    }
    else
    {
        channel_cfg = {
            .gain = ADC_GAIN,
            .reference = ADC_REFERENCE,
            .acquisition_time = ADC_ACQUISITION_TIME,
            .channel_id = channel_id,
            .differential = 0,
        };
        sequence.channels = 0 | BIT(channel_id);
        sequence.buffer = sample_buffer;
        sequence.buffer_size = sizeof(sample_buffer);
        sequence.resolution = ADC_RESOLUTION;

        adc_channel_setup(dev, &channel_cfg);
        adc_vref = adc_ref_internal(dev);
    }
}

void myADC::update_value()
{
    int err;

    if (dev != NULL)
    {
        err = adc_read(dev, &sequence);
        if (err != 0)
        {
            LOG_ERR("ADC reading failed with error %d.", err);
        }
        else
        {
            raw_value = sample_buffer[0];
            if (adc_vref > 0)
            {
                mv_value = raw_value;
                adc_raw_to_millivolts(adc_vref, ADC_GAIN, ADC_RESOLUTION, &mv_value);
            }
        }
    }
}

int32_t myADC::get_value()
{
    return raw_value;
}

int32_t myADC::get_value_mv()
{
    return mv_value;
}