#include "modbus_rtu.h"

void modbus_init(modbus_rtu_t *mb, uint8_t slave_id)
{
    if(mb == NULL) return;
    mb->slave_id = slave_id;
    mb->phy_mode = RS485_MODE_RECEIVE;
    for (int i=0; i<4;i++)
    {
        mb->holding_registers[i] = 0;
    }
}

uint16_t crc16_calculation(const uint8_t *buffer, uint16_t length)
{
    uint16_t crc = 0xFFFF;
    for(uint16_t i = 0; i < length; i++)
    {
        crc ^= (uint16_t)buffer[i];
        for (int bit = 0; bit < 8; bit++)
        {
            bool lsb_set = (crc & 0x0001);
            crc >>=1;
            if(lsb_set){
                crc ^= 0xA001;
            }
        }
        
    }
    return crc;
}

void modbus_update_req(modbus_rtu_t *mb, const control_pilot *cp)
{
    if (mb == NULL || cp == NULL) return;
    mb->holding_registers[REG_PILOT_STATE] = (uint16_t)(cp->state);
    mb->holding_registers[REG_VOLTAGE_MEASURED] = (uint16_t)(cp->measured_voltage * 10.0f);
    mb->holding_registers[REG_MAX_ALLOWEDCURRENT] = (uint16_t)(cp->max_allowed_current * 10.0f);
    mb->holding_registers[REG_FAULT_FLAG] = (uint16_t)(cp->fault_active);
}

bool process_request(modbus_rtu_t *mb, const uint8_t *req_buffer, uint16_t req_length, uint8_t *resp_buf, uint16_t *resp_length)
{
    if(mb == NULL || req_buffer == NULL || resp_buf == NULL || resp_length == NULL) return false;
    if (req_length < 8) return false;

    // if slave id or func code are not matching return 0
    if (req_buffer[0] != mb->slave_id || req_buffer[1] != MODBUS_FC_READ_HOLDING_REGS) return false;
    
    //calculate crc and check with received crc not matching return false
    uint16_t crc_cal = crc16_calculation(req_buffer, req_length-2);
    uint16_t rx_crc = req_buffer[req_length-2] | (req_buffer[req_length-1]<<8);
    if (crc_cal != rx_crc) return false;

    // count check
    uint16_t start_addr = (req_buffer[2] << 8) | req_buffer[3];
    uint16_t reg_cnt = (req_buffer[4]<<8) | req_buffer[5];

    if((start_addr+reg_cnt) > HOLDING_REG_COUNT) return false;

    // Build response packet
    mb->phy_mode = RS485_MODE_TRANSMIT;
    resp_buf[0] = mb->slave_id;
    resp_buf[1] = MODBUS_FC_READ_HOLDING_REGS;
    resp_buf[2] = (uint8_t)(reg_cnt * 2);

    //update the data to response registers
    for(uint16_t index = 0; index<reg_cnt; index++)
    {
        resp_buf[3 + index*2] = (uint8_t)(mb->holding_registers[start_addr+ index]>>8) & 0xFF; //MSB
        resp_buf[4 + index*2] = (uint8_t)(mb->holding_registers[start_addr+index]) & 0xFF;  //LSB
    }

    //update crc along with payload
    uint16_t payload_length = 3 + (reg_cnt*2);
    uint16_t tx_crc = crc16_calculation(resp_buf, payload_length);
    resp_buf[payload_length] = (uint8_t)(tx_crc & 0xFF);
    resp_buf[payload_length+1] = (uint8_t)((tx_crc>>8) & 0xFF);

    *resp_length = payload_length+2;

    return true;

}