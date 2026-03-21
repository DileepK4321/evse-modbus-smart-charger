# Project Plan - Release 1: Foundation (2-Week Sprint)

## Sprint Schedule & Workstreams

| Track | Phase 1: Setup (Days 1-4) | Phase 2: Integration (Days 5-10) | Phase 3: Validation (Days 11-14) |
| :--- | :--- | :--- | :--- |
| **1. Requirements** | Finalize the 10 Core PoC Req IDs (Digital/Analog I/O). | Map Req IDs to Simulink Blocks (Traceability). | Run a "Test Case" to prove [REQ-002] High-Voltage trip. |
| **2. Repo (GitHub)** | Create SafeCharge-Lite, set up `/src`, `/inc`, and `/models`. | Commit the first `.slx` (Simulink) and `.ioc` (STM32 Cube) files. | Document the "Release 1" Tag/Release in GitHub. |
| **3. Hardware** | Unbox Nucleo-F401RE + Wire Potentiometer to PA0. | Flash "Blinky" + ADC Read via MATLAB/Simulink. | Verify the LED toggles at exactly 4.2V (per the Potentiometer). |

## Technical Stack
- **Hardware:** STM32 Nucleo-F401RE
- **IDE:** STM32CubeIDE / MATLAB Simulink
- **Standards:** MISRA C++:2018
- **Version Control:** Git (GitHub)
