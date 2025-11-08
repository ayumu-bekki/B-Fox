[日本語版ドキュメントはこちら](./README.md)

# B-Fox
![B-Fox logo](./docs/logo.svg)

## Overview
B-Fox is an open-source project that recreates and realizes the fun of Fox Hunting using Bluetooth Low Energy (BLE).
The goal is to lower the barriers to entry and participation, making it easy to realize the core enjoyment of radio direction finding: "finding hidden locations by radio signal strength."

When starting this project, we felt that existing Fox Hunting and ARDF had the following challenges that made it not easily accessible:
- High cost of transmitters and receivers
- Requires a license for operating beacons
- Cannot be used commercially as it uses amateur radio bands
- ARDF has strict rules due to its competitive nature

Therefore, we are realizing an easily accessible radio direction finding activity using BLE, which anyone can use without a license.
The protocol uses iBeacon (BLE), so you can play even without a dedicated receiver by installing apps like LightBlue (https://punchthrough.com/lightblue/) on smartphones.

## About Operations
We have created operation documentation (draft) with relaxed rules based on Park ARDF, assuming that this is basically recreation with potential development into ARDF competitions.
[B-Fox Operations & Rules Draft](./operation/OPERATION.md)

## About Transmitters and Receivers
Please refer to the [Transmitter/Receiver Device Documentation](./docs/README.md).
This project includes a complete set of programs and data that can be fabricated with PCB manufacturing and 3D printing.

### Comparison

|                           | ARDF                                                                   | B-Fox              |
| ------------------------- | ---------------------------------------------------------------------- | ------------------ |
| Transmitter/Receiver Cost | $200-700                                                               | $20-70             |
| Qualification             | Third-class amateur radio operator or higher required for beacon setup | Not required       |
| Radio Station License     | Required                                                               | Not required       |
| Receiver                  | Dedicated receiver required                                            | Smartphones work   |
| Detection Method          | Sound, S-meter                                                         | dBm value          |
| Competitiveness           | Established as a competition with world championships                  | Recreation         |
| DIY Receiver Antenna      | Possible                                                               | Generally not      |
| Frequency Used            | 3.5MHz / 144MHz                                                        | 2.4GHz             |
| Antenna Output Power      | 3.5MHz(3-5W) / 144MHz(0.25-1.5W) approx.                               | 0.06-8mW           |
| Activity Range            | Several kilometers                                                     | Several hundred m  |

## Repository Contents
Contains transmitter/receiver programs, mechanical design, and operation methods.

- bfox_receiver
  - Receiver program suite
- bfox_beacon
  - Transmitter program suite
- docs
  - Technical documentation, PCB, Case (for 3D printer)
- operation
  - Operation guidelines and documentation
