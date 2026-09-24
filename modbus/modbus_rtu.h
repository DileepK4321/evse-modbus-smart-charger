#ifndef MODBUS_RTU_H
#define MODBUS_RTU_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "control/control_pilot.h"

#define HOLDING_ADDRESS   0x01
#define HOLDING_REG_COUNT 4

#define MODBUS_FC_READ_HOLDING_REGS 0x03

// RS485 mode
typedef enum 
{
    RS485_MODE_RECEIVE= 0,
    RS485_MODE_TRANSMIT
}rs485_mode_t;

// modbus registers
typedef enum{
    REG_PILOT_STATE=0,
    REG_VOLTAGE_MEASURED,
    REG_MAX_ALLOWEDCURRENT,
    REG_FAULT_FLAG
}modbus_registers_t;

// structure for storing the data
typedef struct {
    uint8_t slave_id;
    uint16_t holding_registers[HOLDING_REG_COUNT];
    rs485_mode_t phy_mode;
}modbus_rtu_t;

void modbus_init(modbus_rtu_t *mb, uint8_t slave_id);
void modbus_update_req(modbus_rtu_t *mb, const control_pilot *cp);
uint16_t crc16_calculation(const uint8_t *buffer, uint16_t length);
bool process_request(modbus_rtu_t *mb, const uint8_t *req_buffer, uint16_t req_length, uint8_t *resp_buf, uint16_t *resp_length);


#endif //MODBUS_RTU_H
