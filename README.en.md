# BleRDF

## Overview
BleRDF is an open-source project that recreates and realizes the fun of amateur radio direction finding (Fox Hunting or ARDF) using Bluetooth Low Energy (BLE).
The goal is to lower the barriers to entry and participation, making it easy to realize the core enjoyment of radio direction finding: "finding hidden locations by radio signal strength."

When starting this project, we felt that ARDF had the following challenges that made it not easily accessible:
- High cost of transmitters and receivers
- Requires a license for operation (beacon setup)
- Cannot be used commercially as it uses amateur radio bands
- Strict rules due to its competitive nature

Therefore, we are realizing an easily accessible radio direction finding activity using BLE (ISM band), which anyone can use without a license.
The protocol uses iBeacon (BLE), so you can play even without a dedicated receiver by installing apps like LightBlue (https://punchthrough.com/lightblue/) on smartphones.

## About Operations

We have created operation documentation (draft) with relaxed rules based on Park ARDF, assuming that this is basically recreation with potential development into ARDF competitions.
[BleRDF Operations & Rules Draft](./operation/OPERATION.md)

## About Transmitters and Receivers

[Transmitter/Receiver Device Documentation](./docs/README.md)

### Comparison

| | ARDF | BleRDF |
| ---- | ---- | ---- |
| Transmitter/Receiver Cost | Tens of thousands of yen | Thousands of yen |
| Qualification | Third-class amateur radio operator or higher required for receiver installation | Not required |
| Radio Station License | Required | Not required |
| Receiver | Dedicated or compatible receiver required | Smartphones possible |
| Detection Method | Auditory (sound) | Visual (numerical value) |
| Competitiveness | Established as a competition with world championships | Recreation |
| DIY Receiver Antenna | Possible | Generally not possible |
| Frequency Used | 3.5MHz / 144MHz | 2.4GHz |
| Antenna Output Power | 3.5MHz(3-5W) / 144MHz(0.25-1.5W) approx. | 0.06-8mW |
| Activity Range | Several kilometers | Several hundred meters |

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
