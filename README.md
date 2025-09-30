# BleRDF

## Overview
This is an attempt to do Fox Hunting (or ARDF) with Bluetooth Low Energy (BLE).
The goal is to lower the barrier to entry and participation, and to realize the core fun of ARDF, which is "searching for a hidden location by the strength of radio waves."

ARDF has several challenges:
- A license is required to set up a beacon.
- Commercial use is not possible because it uses amateur radio bands.
- The cost of transmitters and receivers is high.
- It is not easy to get started and feels like a high hurdle.

Therefore, we are trying to realize similar fun using the general-purpose BLE in the ISM band.

### Comparison
 
| | ARDF | BleRDF |
| ---- | ---- | ---- |
| Cost of transmitter/receiver | Tens of thousands of yen | Several thousand yen |
| Frequency used | 3.5MHz / 144MHz | 2.4GHz |
| Antenna power | 3.5MHz(3-5W) / 144MHz(0.25-1.5W) | 0.06-8mW |
| Range | Several kilometers | Several hundred meters |
| Receiver | Dedicated or compatible receiver required | Smartphone is also possible |
| Detection method on receiver | Sound | Numerical value |
| Receiver antenna modification | Possible | Not possible |
| Competition scale | Competition (world championships) | Play/Game |

## Transmitter/Receiver

The microcontroller used is the Seeed Studio XIAO ESP32C6. It has the following features:
- BLE support
- External antenna can be used (with technical standards conformity certification)
- Small footprint
- Lipo battery support

esp-idf is required to build the program.

The protocol used is iBeacon (BLE).
Therefore, even without a dedicated receiver, you can play by installing an app like LightBlue (https://punchthrough.com/lightblue/) on your smartphone.

## About Operation

Assuming that it is basically a game, but with ARDF competition as a development, we have created operational materials (draft) with relaxed rules based on Park ARDF.


## Repository Contents Description
This repository contains the programs for the transmitter and receiver, mechanical design, and operational methods.

- blerdf_receiver
  - A set of receiver programs
- blerdf_beacon
  - A set of transmitter programs
- docs
  - Technical documents
