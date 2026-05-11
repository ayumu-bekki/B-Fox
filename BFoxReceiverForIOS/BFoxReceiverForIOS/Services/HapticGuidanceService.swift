// HapticGuidanceService.swift
// BFox iBeacon Receiver

import Foundation
import UIKit

/// Provides periodic haptic feedback of the strongest beacon's signal level.
/// Pattern: tap count = beacon minor number, tap intensity = signal strength.
@MainActor
final class HapticGuidanceService {

    // MARK: - Properties

    private var announceTimer: Timer?
    private var tapTimer: Timer?
    private var beaconsProvider: (() -> [DetectedBeacon])?

    // MARK: - Public API

    func start(beaconsProvider: @escaping () -> [DetectedBeacon], interval: TimeInterval) {
        self.beaconsProvider = beaconsProvider
        stopTimer()

        playPattern()
        announceTimer = Timer.scheduledTimer(withTimeInterval: interval, repeats: true) { [weak self] _ in
            Task { @MainActor [weak self] in
                self?.playPattern()
            }
        }
    }

    func stop() {
        stopTimer()
    }

    // MARK: - Private

    private func stopTimer() {
        announceTimer?.invalidate()
        announceTimer = nil
        tapTimer?.invalidate()
        tapTimer = nil
    }

    private func playPattern() {
        guard let beacons = beaconsProvider?(), let strongest = beacons.first else {
            return  // No beacon: no vibration
        }

        let level = VoiceGuidanceService.SignalLevel.from(normalizedRSSI: strongest.normalizedRSSI)
        let tapCount = strongest.minor
        let intensity = hapticIntensity(for: level)

        // Cancel any in-progress taps
        tapTimer?.invalidate()
        tapTimer = nil

        var remaining = tapCount
        let generator = UIImpactFeedbackGenerator(style: .heavy)
        generator.prepare()

        // Fire first tap immediately
        generator.impactOccurred(intensity: CGFloat(intensity))
        remaining -= 1

        if remaining > 0 {
            tapTimer = Timer.scheduledTimer(withTimeInterval: 0.7, repeats: true) { [weak self] timer in
                Task { @MainActor in
                    generator.impactOccurred(intensity: CGFloat(intensity))
                    remaining -= 1
                    if remaining <= 0 {
                        timer.invalidate()
                        self?.tapTimer = nil
                    }
                }
            }
        }
    }

    private func hapticIntensity(for level: VoiceGuidanceService.SignalLevel) -> Float {
        switch level {
        case .veryStrong: return 1.0
        case .strong:     return 0.8
        case .medium:     return 0.6
        case .weak:       return 0.4
        case .veryWeak:   return 0.2
        }
    }
}
