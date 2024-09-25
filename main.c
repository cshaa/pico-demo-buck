#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/vreg.h"

const uint32_t CPU_FREQ = 250000;
const enum vreg_voltage CPU_VOLTAGE = VREG_VOLTAGE_1_25;

const uint32_t PWM_PIN = 0;
const uint32_t PWM_WRAP = 255;
const float duty_cycle = 0.4;

uint32_t time_ms()
{
    return (time_us_32() / 1000);
}

int main()
{
    // Overclocking
    vreg_set_voltage(CPU_VOLTAGE);
    set_sys_clock_khz(CPU_FREQ, true);

    // IO
    stdio_init_all();

    // PWM
    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(PWM_PIN);
    uint chan = pwm_gpio_to_channel(PWM_PIN);

    uint32_t PWM_LEVEL = duty_cycle * (float)PWM_WRAP;
    pwm_set_wrap(slice, PWM_WRAP);
    pwm_set_chan_level(slice, chan, PWM_LEVEL);
    pwm_set_output_polarity(slice, true, true);

    pwm_set_enabled(slice, true);

    // Onboard LED
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    while (true)
    {
        printf("Hello, world! (%i)\n", time_ms() / 1000);
        gpio_put(PICO_DEFAULT_LED_PIN, true);
        sleep_ms(500);
        gpio_put(PICO_DEFAULT_LED_PIN, false);
        sleep_ms(500);
    }
}
