# BleRDF Project Technical Information Overview

BleRDF is an open-source project that recreates and realizes the fun of amateur radio direction finding (ARDF) using Bluetooth Low Energy (BLE).

---

## Main Components and Hardware

### Transmitter/Receiver Main Module

Seeed Studio XIAO ESP32C6

- Low power wireless communication using BLE
- External antenna capable, Technical Conformity Certification (Giteki) obtained
- LiPo battery compatible, suitable power source for portable use
- Small footprint, enables device miniaturization and weight reduction

### Receiver Interface
Character LCD display adopted
LCD Module 16×2 lines AQM1602Y-RN-GBW Available at Akizuki Denshi Tsusho ([https://akizukidenshi.com/catalog/g/g111916/](https://akizukidenshi.com/catalog/g/g111916/))

---

## Software and Communication Protocol

### Development Environment
esp-idf 5.5.1
Espressif IoT Development Framework ([https://github.com/espressif/esp-idf](https://github.com/espressif/esp-idf))

### Communication Protocol
Uses iBeacon (BLE) protocol

Uses GAP to periodically broadcast beacon information
Uses GATT to change transmitter settings and obtain battery voltage
Can be received with smartphone apps like LightBlue ([https://punchthrough.com/lightblue/](https://punchthrough.com/lightblue/)) (in this case, uses GAP device name instead of iBeacon)

---

## Hardware Manufacturing

Case and PCB data are provided for both transmitters and receivers.

- PCB
  - Contains PCB design information. Editable with KiCAD.
  - Boards and cases can be output using services like PCBWay ([https://www.pcbway.com](https://www.pcbway.com)).
    - *_Gerber.zip can be used as-is. (Board size 36x47mm)
- Case
  - Cases that can be output with 3D printers are included. Editable with FreeCAD.

---

## List of Required Main Parts

List of main parts required for reference implementation

### Transmitter

| Item | Notes | Quantity |
| --- | --- | --- |
| Seeed Studio XIAO ESP32C6 |  | 1 |
| LiPo Battery 3000mAh | | 1 |

### Receiver

| Item | Notes | Quantity |
| --- | --- | --- |
| Seeed Studio XIAO ESP32C6 |  | 1 |
| Receiver PCB |  | 1 |
| Receiver Case Board |  | 1 |
| LCD Module AQM1602Y-RN-GBW | | 1 |
| Rod Antenna for XIAO | https://akizukidenshi.com/catalog/g/g117582/ | 1 |
| LiPo Battery 500mAh | | 1 |
| Resistor | 220kΩ | 2 |
|  | 1kΩ| 2 |
| XH Connector | 2P Vertical | 2 |
| Cable with XH Connector | 2P (for ESP32C6 battery connection) | 1 |
| | 2P Horizontal | 1 |
| Capacitor | | |
| Slide Switch | For PCB mount | 1 |
| Tactile Switch | For PCB mount | 1 |
| Polyswitch | 250mA | 1 |
| Split Long Pin Socket | 9pin | 1 |
| | 7pin (Low Profile) | 2 |
| Screw | | |
| Spacer | | |
| Wiring Cord | | Appropriate amount |
