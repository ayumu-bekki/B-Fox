// BeaconModel.swift
// BFox iBeacon Receiver

import Foundation
import CoreLocation

/// A model representing a single detected iBeacon.
struct DetectedBeacon: Identifiable, Equatable {
    let id: Int          // Uses the minor value as the identifier
    let minor: Int
    var rssi: Int        // dBm (negative integer; closer to 0 means stronger signal)
    var proximity: CLProximity
    var lastSeen: Date

    /// Normalizes RSSI to a 0.0–1.0 range for display purposes.
    /// Values at or below rssiFar → 0.0; at or above rssiNear → 1.0.
    var normalizedRSSI: Double {
        let clamped = max(Constants.rssiFar, min(Constants.rssiNear, Double(rssi)))
        let range = Constants.rssiNear - Constants.rssiFar
        return (clamped - Constants.rssiFar) / range
    }

    /// Radius fraction on the radar (0.0 = outer edge, 1.0 = center).
    var radarRadiusFraction: Double {
        return normalizedRSSI
    }

    static func == (lhs: DetectedBeacon, rhs: DetectedBeacon) -> Bool {
        lhs.id == rhs.id &&
        lhs.rssi == rhs.rssi &&
        lhs.proximity == rhs.proximity
    }
}
