#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "control/control_pilot.h"
#include "modbus/modbus_rtu.h"

/* Helper string converter for state printing */
const char* state_to_string(State_transitions state) {
    switch (state) {
        case STATE_A: return "STATE_A (+12V Standby)";
        case STATE_B: return "STATE_B (+9V Connected)";
        case STATE_C: return "STATE_C (+6V Charging)";
        case STATE_E: return "STATE_E (Error)";
        case STATE_F: return "STATE_F (-12V Fault)";
        default: return "UNKNOWN";
    }
}

/* Helper function to print hex buffers */
void print_hex_buffer(const char *label, const uint8_t *buf, uint16_t len) {
    printf("%s [ ", label);
    for (uint16_t i = 0; i < len; i++) {
        printf("0x%02X ", buf[i]);
    }
    printf("]\n");
}

int main(void)
{
    printf("=== EVSE Integrated System: Control Pilot + Modbus RTU ===\n\n");

    /* 1. Initialize Objects */
    control_pilot ctpl;
    init_control(&ctpl);

    modbus_rtu_t mb;
    modbus_init(&mb, 0x01); // Slave ID = 1

    /* 2. Simulate State C (Charging Active at 6V, 50% PWM) */
    printf("[Step 1] Simulating EV connected and requesting power (6.0V, 50%% PWM)...\n");
    for (int tick = 0; tick < 3; tick++) {
        run_control(&ctpl, 6.0f, 50.0f);
    }
    printf("  Control State: %s\n", state_to_string(ctpl.state));
    printf("  Max Allowed Current: %.1f A\n\n", ctpl.max_allowed_current);

    /* 3. Sync State Machine Data into Modbus Holding Registers */
    modbus_update_req(&mb, &ctpl);

    /* 4. Construct Simulated Modbus Master Request Frame */
    /* Query: Read 4 holding registers starting at address 0x0000 */
    uint8_t master_request[8] = {
        0x01, // Slave ID
        0x03, // Function Code: Read Holding Registers
        0x00, 0x00, // Starting Address: 0x0000
        0x00, 0x04  // Quantity of Registers: 4
    };

    /* Calculate and attach CRC to Master Request */
    uint16_t req_crc = crc16_calculation(master_request, 6);
    master_request[6] = (uint8_t)(req_crc & 0xFF);        // CRC LSB
    master_request[7] = (uint8_t)((req_crc >> 8) & 0xFF); // CRC MSB

    print_hex_buffer("[Step 2] Master Request Frame Sent:", master_request, 8);

    /* 5. Process Request in Modbus RTU Slave Engine */
    uint8_t slave_response[256];
    uint16_t resp_len = 0;

    bool success = process_request(&mb, master_request, 8, slave_response, &resp_len);

    /* 6. Display Response and Decode Telemetry */
    if (success) {
        print_hex_buffer("[Step 3] Slave Response Frame Tx:", slave_response, resp_len);

        printf("\n=== Decoded Telemetry Received by Master ===\n");
        uint16_t reg_state   = (slave_response[3] << 8) | slave_response[4];
        uint16_t reg_voltage = (slave_response[5] << 8) | slave_response[6];
        uint16_t reg_current = (slave_response[7] << 8) | slave_response[8];
        uint16_t reg_fault   = (slave_response[9] << 8) | slave_response[10];

        printf("  Register 0x0000 (State):   %u (%s)\n", reg_state, state_to_string((State_transitions)reg_state));
        printf("  Register 0x0001 (Voltage): %.1f V (Raw Register: %u)\n", reg_voltage / 10.0f, reg_voltage);
        printf("  Register 0x0002 (Current): %.1f A (Raw Register: %u)\n", reg_current / 10.0f, reg_current);
        printf("  Register 0x0003 (Fault):   %s (Raw Register: %u)\n", reg_fault ? "ACTIVE" : "NONE", reg_fault);
    } else {
        printf("[ERROR] Modbus request processing failed or CRC error!\n");
    }

    return 0;
}