#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/float.h"
#include "hardware/pwm.h"
#include "hardware/vreg.h"
#include "hardware/adc.h"

const uint32_t CPU_FREQ = 250000;
const enum vreg_voltage CPU_VOLTAGE = VREG_VOLTAGE_1_25;

const uint32_t ADC_PIN = 26;
const uint32_t ADC_CHAN = 0;
const uint32_t ADC_MAX = 1 << 12;
const uintptr_t ADC_MEAS_COUNT = 1 << 10;

const uint32_t PWM_PIN = 0;
const uint32_t PWM_WRAP = 255;

uint32_t time_ms()
{
    return (time_us_32() / 1000);
}

uint16_t read_voltage()
{
    uint64_t meas = 0;
    for (uintptr_t i = 0; i < ADC_MEAS_COUNT; i++)
    {
        meas += adc_read();
    }
    return meas / ADC_MEAS_COUNT;
}

int main()
{
    float target_voltage = 2.1;
    float duty_cycle = target_voltage / 3.3;

    // Overclocking
    vreg_set_voltage(CPU_VOLTAGE);
    set_sys_clock_khz(CPU_FREQ, true);

    // IO
    stdio_init_all();

    // ADC
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(ADC_CHAN);

    // PWM
    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(PWM_PIN);
    uint chan = pwm_gpio_to_channel(PWM_PIN);

    uint32_t pwm_level = duty_cycle * (float)PWM_WRAP;
    pwm_set_wrap(slice, PWM_WRAP);
    pwm_set_chan_level(slice, chan, pwm_level);
    pwm_set_output_polarity(slice, true, true);

    pwm_set_enabled(slice, true);

    // Onboard LED
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    while (true)
    {
        target_voltage = (1 + cosf(time_ms() * 3.1415926f / 10000.0f));

        uint16_t meas = read_voltage();
        float voltage = meas * 3.3f / ADC_MAX;
        duty_cycle += (target_voltage - voltage) / 10.0f;
        if (duty_cycle > 1)
            duty_cycle = 1;
        if (duty_cycle < 0)
            duty_cycle = 0;

        pwm_level = duty_cycle * (float)PWM_WRAP;
        pwm_set_chan_level(slice, chan, pwm_level);

        printf("Target is %f, read %f V, updating duty_cycle to %f\n", target_voltage, voltage, duty_cycle);
        gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN));
        sleep_ms(5);
    }
}
