#include "control_pilot.h"
#include <stddef.h>

#define DEBOUNCE_THRESHOLD_TICKS  3U

void init_control(control_pilot* ctpl)
{
    if(ctpl == NULL) return;

    ctpl->state = STATE_A;  // standby
    ctpl->measured_voltage = 12.0f;     // 12V
    ctpl->max_allowed_current = 0.0f;    // 0A
    ctpl->pwm_duty_cycle = 0.0f; //duty cycle is 0
    ctpl->debounce_counter = 0.0f;
    ctpl->fault_active = false;
}

static float calculate_grid_current(float duty)
{
    if(duty >= 10.0f && duty <= 85.0f)
    {
        return duty*0.6f;
    }
    else if (duty > 85.0f && duty <= 90.0f)
    {
        return (duty - 64.0f)*2.5f;
    }
    else 
    {
        return 0;
    }
}

static State_transitions voltage_transition(float v)
{
    if(v>=10.5f && v<=13.5f) return STATE_A;
    else if (v>=7.0f && v < 10.5f) return STATE_B;
    else if (v>=4.5f && v<7.0f) return STATE_C;
    else if (v>= -1.0f && v <= 2.0f) return STATE_E;
    else if (v<= -10.0f) return STATE_F;
    else return STATE_E;
}

void run_control(control_pilot* ctpl, float raw_adc_volatage, float duty_cycle)
{
    if (ctpl == NULL) return;
    ctpl->measured_voltage = raw_adc_volatage;
    ctpl->pwm_duty_cycle = duty_cycle;

    State_transitions debouncestate = voltage_transition(raw_adc_volatage);

    if(debouncestate != ctpl->state)
    {
        ctpl->debounce_counter++;
        if(ctpl->debounce_counter >= DEBOUNCE_THRESHOLD_TICKS)
        {
            ctpl->state = debouncestate;
            ctpl->debounce_counter = 0;
        }
        else ctpl->debounce_counter = 0;
    }
    
    switch (ctpl->state)
    {
    case STATE_A:
    case STATE_B:
        ctpl->max_allowed_current = 0.0f;
        ctpl->fault_active = false;
        break;
    case STATE_C:
        ctpl->max_allowed_current = calculate_grid_current(duty_cycle);
        ctpl->fault_active = false;
        break;
    case STATE_E:
    case STATE_F:    
    default:
        ctpl->max_allowed_current = 0.0f;
        ctpl->fault_active = true;
        break;
    }
}