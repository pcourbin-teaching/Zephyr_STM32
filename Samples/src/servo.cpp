#include "servo.hpp"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app_servo);

void myServo::init(const pwm_dt_spec *servo_p, const uint32_t min_pulse_p, const uint32_t max_pulse_p)
{
    servo = servo_p;
    min_pulse = min_pulse_p;
    max_pulse = max_pulse_p;

    if (!pwm_is_ready_dt(servo))
    {
        LOG_ERR("PWM device is not ready");
    }
}

int myServo::setPulse(uint32_t pulse)
{
    int ret = pwm_set_pulse_dt(servo, pulse);
    current_pulse = pulse;
    return ret;
}

int myServo::setPosition(int8_t position)
{
    uint32_t pulse = 0;
    pulse = ((position - (-90)) * (max_pulse - min_pulse)) / (90 - (-90)) + min_pulse;
    return setPulse(pulse);
}

int myServo::getPosition()
{
    return ((current_pulse - min_pulse) * (90 - (-90)) / (max_pulse - min_pulse)) + (-90);
}