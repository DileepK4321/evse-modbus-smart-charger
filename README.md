```markdown
# EVSE Smart Charger Firmware (IEC 61851 & Modbus RTU)

A bare-metal, zero-dependency ANSI C (C99) firmware implementation for Electric Vehicle Supply Equipment (EVSE). Features a debounced IEC 61851-1 Control Pilot state machine integrated with an industrial Modbus RTU slave engine over RS-485 with bitwise CRC-16 hardware data protection.

---

## Technical Highlights

- **IEC 61851-1 Control Pilot State Machine:** Real-time state detection for State A (+12V Standby), State B (+9V EV Connected), State C (+6V Charging Active), State E (0V Error), and State F (-12V Fault).
- **Tick Debouncing Filter:** Implements consecutive-sample noise filtering ($300\,\mu\text{s}$ hysteresis window assuming a $100\,\mu\text{s}$ ISR tick) to suppress ADC switching noise.
- **Dynamic Current Scaling:** Maps incoming PWM duty cycle ($10\%$ to $90\%$) directly to maximum allowable grid current ($6\,\text{A}$ to $80\,\text{A}$) according to IEC 61851-1 specs.
- **Modbus RTU Telemetry Stack:** Implements Function Code `0x03` (Read Holding Registers) with bitwise CRC-16 calculation using the standard `0xA001` reverse polynomial.
- **Fixed-Point Data Engine:** Converts floating-point ADC measurements (Voltage, Current) into 16-bit integer registers for low-overhead RS-485 transmission.
- **RS-485 Hardware Direction Control:** Toggles Driver Enable / Receiver Enable (`DE/RE`) physical transceiver modes during request/response cycles.

---

## Repository Structure

```text
evse-modbus-smart-charger/
├── main.c                   # Integrated test harness & Modbus master simulator
├── control/
│   ├── control_pilot.h      # Control pilot data structures & public API
│   └── control_pilot.c      # IEC 61851 state transitions & PWM math
└── modbus/
    ├── modbus_rtu.h         # Register map enums & Modbus slave API
    └── modbus_rtu.c         # Bitwise CRC-16 calculation & FC 0x03 parser

```

---

## Modbus Holding Register Map (Function Code 0x03)

| Address | Register Name | Data Type | Scale Factor | Range / Description |
| --- | --- | --- | --- | --- |
| `0x0000` | `REG_PILOT_STATE` | `uint16_t` | 1:1 | `0` = State A, `1` = State B, `2` = State C, `3` = State E, `4` = State F |
| `0x0001` | `REG_VOLTAGE_MEASURED` | `uint16_t` | x10 | Measured Pilot Voltage ($120 = 12.0\,\text{V}$, $60 = 6.0\,\text{V}$) |
| `0x0002` | `REG_MAX_ALLOWED_CURRENT` | `uint16_t` | x10 | Computed Grid Current Limit ($300 = 30.0\,\text{A}$) |
| `0x0003` | `REG_FAULT_FLAG` | `uint16_t` | 1:1 | Safety Status ($0$ = Normal Operation, $1$ = Active Fault) |

---

## Control Pilot Mapping Logic (IEC 61851-1)

### Voltage State Boundaries

* **State A ($+12\,\text{V}$ nominal):** $+10.5\,\text{V}$ to $+13.5\,\text{V}$ (Standby)
* **State B ($+9\,\text{V}$ nominal):** $+7.0\,\text{V}$ to $+10.5\,\text{V}$ (EV Connected, Not Ready)
* **State C ($+6\,\text{V}$ nominal):** $+4.5\,\text{V}$ to $+7.0\,\text{V}$ (EV Connected, Charging)
* **State E ($0\,\text{V}$ nominal):** $-1.0\,\text{V}$ to $+2.0\,\text{V}$ (Error / Ground Short)
* **State F ($-12\,\text{V}$ nominal):** $\le -10.0\,\text{V}$ (Fault / EVSE Unavailable)

### PWM Duty Cycle to Current Formula

* $10\% \le \text{Duty} \le 85\% \implies \text{Current (A)} = \text{Duty (\%)} \times 0.6$
* $85\% < \text{Duty} \le 90\% \implies \text{Current (A)} = (\text{Duty (\%)} - 64) \times 2.5$

---

## Compilation & Execution

Compile using any standard C99 compiler (GCC/Clang):

```bash
# Compile project sources
gcc -Wall -Wextra -I. main.c control/control_pilot.c modbus/modbus_rtu.c -o evse_sim

# Run simulation executable
./evse_sim

```

### Sample Verification Output

```text
=== EVSE Integrated System: Control Pilot + Modbus RTU ===

[Step 1] Simulating EV connected and requesting power (6.0V, 50% PWM)...
  Control State: STATE_C (+6V Charging)
  Max Allowed Current: 30.0 A

[Step 2] Master Request Frame Sent: [ 0x01 0x03 0x00 0x00 0x00 0x04 0x44 0x09 ]
[Step 3] Slave Response Frame Tx: [ 0x01 0x03 0x08 0x00 0x02 0x00 0x3C 0x01 0x2C 0x00 0x00 0x26 0x27 ]

=== Decoded Telemetry Received by Master ===
  Register 0x0000 (State):   2 (STATE_C (+6V Charging))
  Register 0x0001 (Voltage): 6.0 V (Raw Register: 60)
  Register 0x0002 (Current): 30.0 A (Raw Register: 300)
  Register 0x0003 (Fault):   NONE (Raw Register: 0)

```

```

```
