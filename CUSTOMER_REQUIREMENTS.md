# Customer Requirement Specification (CRS) - Release 1: Safety Baseline

## 1. Scope
This document defines the functional and safety requirements for the initial Proof of Concept (PoC) of the SafeCharge-Lite controller. The focus is on Hardware Abstraction and Core Safety Logic.

## 2. Functional Requirements
| ID | Requirement Description | Priority |
| :--- | :--- | :--- |
| **REQ-CUST-001** | **Real-Time Sensing:** The system shall measure a simulated battery cell voltage (via potentiometer) with a resolution of at least 10 bits. | High |
| **REQ-CUST-002** | **High-Voltage Protection:** The system shall trigger a "Critical Fault" state if the sensed voltage exceeds 4.2V. | High |
| **REQ-CUST-003** | **Low-Voltage Protection:** The system shall trigger a "Critical Fault" state if the sensed voltage drops below 3.0V. | High |
| **REQ-CUST-004** | **Fault Debouncing:** A fault condition must be sustained for a minimum of 500ms before the system enters a Safe State to prevent EMI-induced tripping. | High |
| **REQ-CUST-005** | **Safe State Execution:** Upon Critical Fault, the system shall de-energize the charging output (Onboard LED LD2) and latch until reset. | High |
| **REQ-CUST-006** | **Visual Status Feedback:** The system shall provide visual indicators for: Standby (Blinking), Charging (Solid), and Fault (Rapid Flash). | Medium |
| **REQ-CUST-007** | **Thermal Monitoring:** The system shall support an auxiliary input for temperature monitoring and trip the Safe State if limits are exceeded. | Medium |
| **REQ-CUST-008** | **Deterministic Execution:** Core BMS logic must execute at a fixed task frequency of 100ms. | High |
| **REQ-CUST-009** | **HMI / Debug:** The system shall output Voltage, State, and Fault logs to PC via UART. | Medium |
| **REQ-CUST-010** | **Power-On Self-Test (POST):** The system shall verify ADC and memory integrity upon startup. | Medium |
