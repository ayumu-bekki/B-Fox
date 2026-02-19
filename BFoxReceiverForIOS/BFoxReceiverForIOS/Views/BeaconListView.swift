// BeaconListView.swift
// BFox iBeacon Receiver

import SwiftUI
import CoreLocation

/// A summary list of beacons sorted by descending RSSI.
struct BeaconListView: View {

    let beacons: [DetectedBeacon]
    let showRSSI: Bool

    var body: some View {
        if beacons.isEmpty {
            emptyState
        } else {
            ScrollView {
                VStack(spacing: 8) {
                    ForEach(beacons, id: \.id) { beacon in
                        BeaconRowView(beacon: beacon, showRSSI: showRSSI)
                    }
                }
                .padding(.horizontal, 16)
                .padding(.vertical, 8)
            }
        }
    }

    private var emptyState: some View {
        VStack(spacing: 12) {
            Image(systemName: "antenna.radiowaves.left.and.right.slash")
                .font(.system(size: 36))
                .foregroundStyle(.secondary)
            Text(String(localized: "beacon.not_detected"))
                .font(.subheadline)
                .foregroundStyle(.secondary)
        }
        .frame(maxWidth: .infinity, maxHeight: .infinity)
    }
}

// MARK: - BeaconRowView

private struct BeaconRowView: View {

    let beacon: DetectedBeacon
    let showRSSI: Bool

    var body: some View {
        let color   = Constants.beaconColor(for: beacon.minor)
        let opacity = 0.3 + beacon.normalizedRSSI * 0.7

        HStack(spacing: 10) {
            // Identifier icon
            Image(systemName: "wifi")
                .font(.system(size: 20))
                .foregroundStyle(color)
                .frame(width: 28)

            // Minor number
            Text("\(beacon.minor)")
                .font(.system(size: 18, weight: .semibold, design: .monospaced))
                .foregroundStyle(color)

            // Proximity label
            Text(proximityLabel(beacon.proximity))
                .font(.caption)
                .foregroundStyle(.secondary)

            Spacer()

            // RSSI bar + value
            HStack(spacing: 8) {
                RSSIBarView(normalizedValue: beacon.normalizedRSSI, color: color.opacity(opacity))
                    .frame(width: 60, height: 16)

                if showRSSI {
                    Text("\(beacon.rssi) dBm")
                        .font(.system(size: 12, weight: .medium, design: .monospaced))
                        .foregroundStyle(.secondary)
                        .frame(width: 72, alignment: .trailing)
                }
            }
        }
        .padding(.horizontal, 14)
        .padding(.vertical, 10)
        .background(
            RoundedRectangle(cornerRadius: 10)
                .fill(Color(.systemGray6))
        )
        .overlay(
            RoundedRectangle(cornerRadius: 10)
                .strokeBorder(color.opacity(0.4), lineWidth: 1)
        )
    }

    // MARK: - Helpers

    private func proximityLabel(_ proximity: CLProximity) -> String {
        switch proximity {
        case .immediate: return String(localized: "proximity.immediate")
        case .near:      return String(localized: "proximity.near")
        case .far:       return String(localized: "proximity.far")
        default:         return String(localized: "proximity.unknown")
        }
    }
}

// MARK: - RSSIBarView

/// A horizontal progress bar showing RSSI signal strength.
private struct RSSIBarView: View {

    let normalizedValue: Double   // 0.0–1.0
    let color: Color

    var body: some View {
        GeometryReader { geo in
            ZStack(alignment: .leading) {
                // Track
                RoundedRectangle(cornerRadius: 3)
                    .fill(Color(.systemGray4))

                // Fill
                RoundedRectangle(cornerRadius: 3)
                    .fill(color)
                    .frame(width: geo.size.width * CGFloat(max(0, min(1, normalizedValue))))
                    .animation(.spring(response: 0.3), value: normalizedValue)
            }
        }
    }
}

// MARK: - Preview

#Preview {
    BeaconListView(
        beacons: [
            DetectedBeacon(id: 1, minor: 1, rssi: -55, proximity: .immediate, lastSeen: Date()),
            DetectedBeacon(id: 2, minor: 2, rssi: -72, proximity: .near,      lastSeen: Date()),
            DetectedBeacon(id: 3, minor: 3, rssi: -88, proximity: .far,       lastSeen: Date()),
        ],
        showRSSI: true
    )
}
