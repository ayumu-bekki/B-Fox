// RadarView.swift
// BFox iBeacon Receiver

import SwiftUI
import CoreLocation

/// Displays beacons in a horizontal row, representing distance by dot size.
struct RadarView: View {

    let beacons: [DetectedBeacon]
    let showRSSI: Bool

    @State private var scanPhase: Int = 0

    var body: some View {
        GeometryReader { geo in
            let maxDotSize = min(geo.size.height * 0.85, 100.0)
            let minDotSize = maxDotSize * 0.25

            ZStack {
                if beacons.isEmpty {
                    scanningIndicator
                        .frame(width: geo.size.width, height: geo.size.height)
                        .transition(.opacity)
                } else {
                    let ordered: [DetectedBeacon] = {
                        var left: [DetectedBeacon] = []
                        var center: [DetectedBeacon] = []
                        var right: [DetectedBeacon] = []
                        for (i, beacon) in beacons.enumerated() {
                            if i == 0 {
                                center.append(beacon)
                            } else if i % 2 == 1 {
                                right.append(beacon)
                            } else {
                                left.insert(beacon, at: 0)
                            }
                        }
                        return left + center + right
                    }()

                    // TimelineView provides per-frame timestamps used as floatTime for animation.
                    TimelineView(.animation) { context in
                        let floatTime = context.date.timeIntervalSinceReferenceDate

                        HStack(spacing: 8) {
                            ForEach(ordered) { beacon in
                                let fraction = CGFloat(beacon.radarRadiusFraction)
                                let dotSize  = minDotSize + fraction * (maxDotSize - minDotSize)
                                beaconDot(beacon: beacon, dotSize: dotSize, floatTime: floatTime)
                            }
                        }
                        .frame(width: geo.size.width, height: geo.size.height)
                    }
                    .transition(.opacity)
                    .animation(.spring(response: 0.4, dampingFraction: 0.7), value: beacons.map { $0.rssi })
                }
            }
            .animation(.easeInOut(duration: 0.4), value: beacons.isEmpty)
        }
    }

    // MARK: - Scanning Indicator

    private var scanningIndicator: some View {
        HStack(spacing: 14) {
            ForEach(0..<3, id: \.self) { index in
                Circle()
                    .fill(Color.secondary.opacity(scanPhase == index ? 0.8 : 0.2))
                    .frame(width: 8, height: 8)
                    .scaleEffect(scanPhase == index ? 1.3 : 1.0)
                    .animation(
                        .easeInOut(duration: 0.4).delay(Double(index) * 0.15),
                        value: scanPhase
                    )
            }
        }
        // task runs only while the view is visible and is automatically cancelled on disappear.
        .task {
            while !Task.isCancelled {
                try? await Task.sleep(for: .milliseconds(500))
                scanPhase = (scanPhase + 1) % 3
            }
        }
    }

    // MARK: - Subviews

    @ViewBuilder
    private func beaconDot(beacon: DetectedBeacon, dotSize: CGFloat, floatTime: Double) -> some View {
        let phaseOffset = Double(beacon.minor) * 0.9
        let xOffset     = CGFloat(sin(floatTime * 0.7 + phaseOffset)       * 4.0)
        let yOffset     = CGFloat(sin(floatTime * 1.1 + phaseOffset + 1.0) * 5.0)
        let color       = Constants.beaconColor(for: beacon.minor)
        // Opacity encodes signal strength (weak = 0.35, strong = 1.0).
        let opacity     = 0.3 + beacon.normalizedRSSI * 0.7

        ZStack {
            // Glow
            Circle()
                .fill(color.opacity(opacity * 0.35))
                .frame(width: dotSize + 12, height: dotSize + 12)

            // Dot body
            Circle()
                .fill(color.opacity(opacity))
                .frame(width: dotSize, height: dotSize)
                .overlay(
                    Circle()
                        .strokeBorder(Color.white.opacity(0.5), lineWidth: 1)
                )

            // Label
            VStack(spacing: -4) {
                Text("\(beacon.minor)")
                    .font(.system(size: max(13, dotSize * 0.42), weight: .bold, design: .monospaced))
                    .foregroundStyle(.white)
                    .shadow(color: .black.opacity(0.8), radius: 1, x: 0, y: 0)
                if showRSSI {
                    Text("\(beacon.rssi)")
                        .font(.system(size: max(8, dotSize * 0.22), weight: .regular, design: .monospaced))
                        .foregroundStyle(.white.opacity(0.85))
                        .shadow(color: .black.opacity(0.8), radius: 1, x: 0, y: 0)
                }
            }
        }
        .offset(x: xOffset, y: yOffset)
        .transition(.opacity.combined(with: .scale))
    }
}

// MARK: - Preview

#Preview("Scanning (no beacons)") {
    RadarView(beacons: [], showRSSI: false)
        .frame(height: 160)
        .padding(24)
}

#Preview("3 beacons") {
    RadarView(
        beacons: [
            DetectedBeacon(id: 1, minor: 1, rssi: -55, proximity: .immediate, lastSeen: Date()),
            DetectedBeacon(id: 2, minor: 2, rssi: -72, proximity: .near,      lastSeen: Date()),
            DetectedBeacon(id: 3, minor: 3, rssi: -88, proximity: .far,       lastSeen: Date()),
        ],
        showRSSI: false
    )
    .frame(height: 160)
    .padding(24)
}

#Preview("1 beacon") {
    RadarView(
        beacons: [
            DetectedBeacon(id: 1, minor: 1, rssi: -60, proximity: .near, lastSeen: Date()),
        ],
        showRSSI: false
    )
    .frame(height: 160)
    .padding(24)
}
