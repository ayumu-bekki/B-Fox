# BleRDF

## Overview
An attempt to do Fox Hunting (or ARDF) with Bluetooth Low Energy (BLE).

ARDF has several challenges:
- A license is required to set up a beacon.
- Commercial use is not possible because it uses amateur radio bands.
- The cost of transmitters and receivers is high.
- It's not easy and feels like a high hurdle.

Therefore, we are trying to achieve a similar game using the common and ISM band BLE.

### Comparison
 
| | ARDF | BleRDF |
| ---- | ---- | ---- |
| Transmitter/Receiver Cost | Tens of thousands of yen | Several thousand yen |
| Frequency Used | 3.5MHz / 144MHz | 2.4GHz |
| Antenna Power | Around 3.5MHz(3-5W) / 144MHz(0.25-1.5W) | 0.06-8mW |
| Range | Several kilometers | Several hundred meters |
| Detection Method | Sound | Numerical value |
| Receiver Antenna Modification | Possible | Not possible |
| Competition Scale | World championships exist | Just a game |

## Repository Contents
This repository contains the programs for the transmitter and receiver, as well as mechanical design and operational methods.

- operation
 - Operational documents
- brdf_receiver
 - Complete receiver program
- brdf_beacon
 - Complete transmitter program
- docs
 - Technical documents
