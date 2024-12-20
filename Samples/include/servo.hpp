#ifndef SERVO_HPP
#define SERVO_HPP

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>

class myServo
{

public:
    const pwm_dt_spec *servo;
    uint32_t min_pulse;
    uint32_t max_pulse;
    uint32_t current_pulse;

    void init(const pwm_dt_spec *servo, uint32_t min_pulse, uint32_t max_pulse);
    int setPulse(uint32_t pulse);
    int setPosition(int8_t position);
    int getPosition();
};

#endif