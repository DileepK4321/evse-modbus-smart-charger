#ifndef CONTROL_PILOT_H
#define CONTROL_PILOT_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    STATE_A = 0, //12v standby
    STATE_B,     // 9V connected
    STATE_C,     // 6V ready to charge
    STATE_E,     // Error
    STATE_F      // Fault
}State_transitions;

typedef struct 
{
    State_transitions state;
    float measured_voltage;
    float pwm_duty_cycle;
    float max_allowed_current;
    uint8_t debounce_counter;
    bool fault_active;
}control_pilot;

void init_control(control_pilot* ctpl);
void run_control(control_pilot* ctpl, float raw_adc_volatage, float duty_cycle);


#endif //CONTROL_PILOT_H