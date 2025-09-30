# BleRDF

## Overview
An attempt to implement Fox Hunting (or ARDF) using Bluetooth Low Energy (BLE).
The goal is to lower the barriers to entry and participation, realizing the core enjoyment of ARDF: "finding hidden locations by radio signal strength."

ARDF has several challenges:
- Requires a license to set up beacons
- Cannot be used commercially as it uses amateur radio bands
- High cost of transmitters and receivers
- Not user-friendly and feels like a high hurdle

Therefore, we are trying to realize similar activities using the ISM band and common BLE.

### Comparison

| | ARDF | BleRDF |
| ---- | ---- | ---- |
| Transmitter/Receiver Cost | Tens of thousands of yen | Thousands of yen |
| Frequency Used | 3.5MHz / 144MHz | 2.4GHz |
| Antenna Output Power | 3.5MHz(3-5W) / 144MHz(0.25-1.5W) approx. | 0.06-8mW |
| Activity Range | Several kilometers | Several hundred meters |
| Receiver | Dedicated or compatible receiver required | Smartphones possible |
| Detection Method on Receiver | Sound | Numerical value |
| Receiver Antenna Modification | Possible | Not possible |
| Competition Scale | Competition (with world championships) | Recreation |

## Transmitter/Receiver

Uses Seeed Studio XIAO ESP32C6 microcontroller. It has the following features:
- BLE compatible
- External antenna available (with technical certification)
- Small footprint
- Rechargeable battery compatible

esp-idf is required to build the programs.

The protocol uses iBeacon (BLE).
Therefore, even without a dedicated receiver, you can play by installing apps like LightBlue (https://punchthrough.com/lightblue/) on smartphones.

## About Operations

We have created operation documentation (draft) with relaxed rules based on Park ARDF, assuming that this is basically recreation but with potential development into ARDF competitions.

[BleRDF Operations & Rules Draft](./operation/OPERATION.en.md)


## Repository Contents
Contains transmitter/receiver programs, mechanical design, and operation methods.

- blerdf_receiver
  - Receiver program suite
- blerdf_beacon
  - Transmitter program suite
- docs
  - Technical documentation
- operation
  - Operations kit
