// Constants.swift
// BFox iBeacon Receiver

import Foundation
import SwiftUI

enum Constants {
    // iBeacon Proximity UUID
    // Source: kBFoxIBeaconProximityUuid in bfox_beacon.cc
    // Bytes: C6 5B 2C 5D - 9E 53 - 46 EC - 8B 8E - 54 D9 E2 F2 11 88
    static let beaconUUID = UUID(uuidString: "C65B2C5D-9E53-46EC-8B8E-54D9E2F21188")!

    // AppStorage keys
    static let majorKey = "beacon_major"
    static let showRSSIKey = "show_rssi"

    // Default values
    static let defaultMajor = 0

    // Radar display
    static let radarZoneCount = 3       // Number of concentric circles
    static let beaconDotSize: CGFloat = 36
    static let radarPadding: CGFloat = 16

    // RSSI thresholds for zone mapping (dBm)
    static let rssiNear: Double = -30
    static let rssiFar: Double  = -90

    // Per-minor identifier colors (shuffled so adjacent values have distinct hues)
    private static let beaconPalette: [Color] = [
        Color(hue: 0.17, saturation: 0.80, brightness: 0.95),  // Yellow
        Color(hue: 0.60, saturation: 0.80, brightness: 0.95),  // Blue
        Color(hue: 0.90, saturation: 0.80, brightness: 0.95),  // Pink
        Color(hue: 0.75, saturation: 0.80, brightness: 0.95),  // Purple
        Color(hue: 0.38, saturation: 0.80, brightness: 0.95),  // Green
        Color(hue: 0.08, saturation: 0.80, brightness: 0.95),  // Orange
        Color(hue: 0.50, saturation: 0.80, brightness: 0.95),  // Cyan
        Color(hue: 0.00, saturation: 0.80, brightness: 0.95),  // Red
        Color(hue: 0.70, saturation: 0.80, brightness: 0.95),  // Blue-purple
        Color(hue: 0.28, saturation: 0.80, brightness: 0.95),  // Yellow-green
    ]

    static func beaconColor(for minor: Int) -> Color {
        beaconPalette[minor % beaconPalette.count]
    }
}
