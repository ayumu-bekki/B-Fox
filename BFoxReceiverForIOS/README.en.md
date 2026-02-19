# B-Fox Receiver for iOS

An iOS app that visually displays the distance to iBeacons in real time.

> Japanese documentation: [README.md.ja](README.md.ja)

## Overview

B-Fox Receiver scans for iBeacons with a fixed Proximity UUID and shows each detected beacon as a dot whose **size represents signal strength** — the closer the beacon, the larger the dot. A signal-strength bar and optional dBm readout are shown in the list below.

## Features

- Real-time iBeacon ranging via CoreLocation
- Visual dot display: dot size and opacity scale with RSSI
- Floating animation per beacon (unique phase per minor value)
- Beacon list sorted by descending RSSI with proximity label and bar
- Scanning indicator (animated dots) when no beacons are detected
- Major value selector (0–9) to filter beacons by group
- Optional RSSI dBm overlay on dots and list rows
- Location permission denied/restricted state handled with a full-screen prompt
- English / Japanese localization

## Requirements

- iOS 18.6+
- Xcode 16+
- A physical iOS device (iBeacon ranging is not supported in Simulator)

## Getting Started

### 1. Configure your Team ID

Copy the sample config file and fill in your Apple Developer Team ID:

```bash
cp BFoxReceiverForIOS/Config/Local.xcconfig.sample \
   BFoxReceiverForIOS/Config/Local.xcconfig
```

Open `Local.xcconfig` and replace `YOUR_TEAM_ID`:

```
DEVELOPMENT_TEAM = YOUR_TEAM_ID
```

Your Team ID can be found at [developer.apple.com](https://developer.apple.com) under **Membership**, or in **Xcode → Settings → Accounts**.

### 2. Assign the xcconfig in Xcode

1. Open `BFoxReceiverForIOS.xcodeproj` in Xcode.
2. Select the project root in the Project Navigator, then open the **Info** tab.
3. Under **Configurations**, set both **Debug** and **Release** for the `BFoxReceiverForIOS` target to **Base**.

### 3. Build and run

Select a connected device and press **Run** (⌘R).

## Project Structure

```
BFoxReceiverForIOS/
├── Models/
│   ├── BeaconModel.swift       # DetectedBeacon struct
│   └── Constants.swift         # UUID, RSSI thresholds, colors
├── Services/
│   └── BeaconScanner.swift     # CLLocationManager wrapper (@MainActor)
├── Views/
│   ├── RadarView.swift         # Dot-based visual display
│   ├── BeaconListView.swift    # RSSI-sorted list with bar indicator
│   └── SettingsView.swift      # Major selector and display options
├── Config/
│   ├── Base.xcconfig           # Shared build settings (tracked)
│   ├── Local.xcconfig          # Personal settings — Team ID (not tracked)
│   └── Local.xcconfig.sample   # Template for Local.xcconfig
├── ContentView.swift           # Root view with TabView
├── BFoxReceiverForIOSApp.swift # App entry point
└── Localizable.xcstrings       # English / Japanese strings
```

## Beacon Configuration

| Setting | Value |
|---------|-------|
| Proximity UUID | `C65B2C5D-9E53-46EC-8B8E-54D9E2F21188` |
| Major | Configurable in Settings (0–9) |
| Minor | Used as the beacon identifier and display label |

The UUID is the value defined for B-Fox on the beacon side.
