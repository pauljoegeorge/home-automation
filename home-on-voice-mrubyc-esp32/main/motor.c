#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_attr.h"

#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"
#include "mrubyc.h"

//You can get these value from the datasheet of servo you use, in general pulse width varies between 1000 to 2000 mocrosecond
#define SERVO_MIN_PULSEWIDTH 1000 //Minimum pulse width in microsecond
#define SERVO_MAX_PULSEWIDTH 2000 //Maximum pulse width in microsecond
#define SERVO_MAX_DEGREE 90 //Maximum angle in degree upto which servo can rotate
uint32_t angle, count;
mcpwm_config_t pwm_config;

void c_servo_gpio_initialize(mrb_vm *vm, mrb_value *v, int argc)
{
    int pin = GET_INT_ARG(1);
    printf("the pin is .... %d\n", pin);
    printf("initializing mcpwm servo control gpio......\n");
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, pin);    //Set GPIO 18 as PWM0A, to which servo is connected
}

void c_servo_params_initialize(mrb_vm *vm, mrb_value *v, int argc)
{
  int frequency = GET_INT_ARG(1);
  int duty_cycle_a = GET_INT_ARG(2);
  int duty_cycle_b = GET_INT_ARG(3);
  printf("Configuring Initial Parameters of mcpwm......\n");
  printf("the frequency is .... %d\n", frequency);

  pwm_config.frequency = frequency;    //frequency = 50Hz, i.e. for every servo motor time period should be 20ms
  pwm_config.cmpr_a = duty_cycle_a;    //duty cycle of PWMxA = 0
  pwm_config.cmpr_b = duty_cycle_b;    //duty cycle of PWMxb = 0
  pwm_config.counter_mode = MCPWM_UP_COUNTER;
  pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);    //Configure PWM0A & PWM0B with above settings
}

void c_run_servo_motor(mrb_vm *vm, mrb_value *v, int argc)
{
  angle = GET_INT_ARG(1);
  mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, angle);
}
