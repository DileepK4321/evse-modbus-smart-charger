# SafeChargerLite_development
Markdown# EVSE Smart Charger Firmware (IEC 61851 & Modbus RTU)

A bare-metal, zero-dependency ANSI C (C99) production-grade firmware module implementing the IEC 61851-1 Control Pilot state machine and a Modbus RTU slave engine over RS-485 with CRC-16 hardware integrity checks.

## Key Features

- **IEC 61851-1 Control Pilot Engine:** Implements State A (+12V), State B (+9V), State C (+6V), State E (Error), and State F (Fault) state machine with debounced ADC processing.
- **Dynamic Current Scaling:** Maps incoming PWM duty cycle (10% to 90%) to maximum grid current limit in Amperes per IEC standards.
- **Modbus RTU Telemetry Stack:** Supports Function Code `0x03` (Read Holding Registers) with bitwise CRC-16 calculation (`0xA001` polynomial).
- **Fixed-Point Conversion:** Scales floating-point ADC measurements (Voltage, Current) to 16-bit registers for low-overhead industrial communication.
- **Transceiver Direction Control:** Manages RS-485 Driver Enable / Receiver Enable (`DE/RE`) physical interface modes.

## Repository Structure

```text
├── main.c                   # Integrated simulation harness & test runner
├── control/
│   ├── control_pilot.h      # State machine definitions & API
│   └── control_pilot.c      # Debounce logic & IEC current calculation
└── modbus/
    ├── modbus_rtu.h         # Register map & Modbus stack API
    └── modbus_rtu.c         # Bitwise CRC-16 & frame processing engine

## Modbus Holding Register Map (Function Code 0x03)

| Address | Parameter | Type | Scale | Range / Description |
| :--- | :--- | :--- | :--- | :--- |
| `0x0000` | `REG_PILOT_STATE` | `uint16_t` | 1:1 | `0` = State A, `1` = State B, `2` = State C, `3` = State E, `4` = State F |
| `0x0001` | `REG_VOLTAGE_MEASURED` | `uint16_t` | x10 | Measured Pilot Voltage ($120 = 12.0\,\text{V}$, $60 = 6.0\,\text{V}$) |
| `0x0002` | `REG_MAX_ALLOWED_CURRENT` | `uint16_t` | x10 | Dynamic Grid Current Limit ($300 = 30.0\,\text{A}$) |
| `0x0003` | `REG_FAULT_FLAG` | `uint16_t` | 1:1 | System Safety Status ($0$ = Normal, $1$ = Active Fault) |

 RunCompile using any standard C99 compiler (GCC/Clang):Bashgcc -Wall -Wextra -I. main.c control/control_pilot.c modbus/modbus_rtu.c -o evse_sim
./evse_sim
