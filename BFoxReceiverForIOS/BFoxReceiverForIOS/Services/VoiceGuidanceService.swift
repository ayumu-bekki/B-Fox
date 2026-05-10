// VoiceGuidanceService.swift
// BFox iBeacon Receiver

import Foundation
import AVFoundation

/// Provides periodic voice announcements of the strongest beacon's signal level.
@MainActor
final class VoiceGuidanceService {

    // MARK: - Signal Level (5 stages)

    enum SignalLevel: Int, CaseIterable {
        case veryWeak = 1
        case weak = 2
        case medium = 3
        case strong = 4
        case veryStrong = 5

        /// Determine level from normalized RSSI (0.0–1.0)
        static func from(normalizedRSSI: Double) -> SignalLevel {
            switch normalizedRSSI {
            case 0.8...: return .veryStrong
            case 0.6..<0.8: return .strong
            case 0.4..<0.6: return .medium
            case 0.2..<0.4: return .weak
            default: return .veryWeak
            }
        }

        func localizedName() -> String {
            switch self {
            case .veryStrong: return String(localized: "voice.level.verystrong")
            case .strong:     return String(localized: "voice.level.strong")
            case .medium:     return String(localized: "voice.level.medium")
            case .weak:       return String(localized: "voice.level.weak")
            case .veryWeak:   return String(localized: "voice.level.veryweak")
            }
        }
    }

    // MARK: - Properties

    private let synthesizer = AVSpeechSynthesizer()
    private var announceTimer: Timer?
    private var beaconsProvider: (() -> [DetectedBeacon])?
    private var noSignalPlayer: AVAudioPlayer?

    /// Announcement interval in seconds
    var interval: TimeInterval = 5.0


    // MARK: - Public API

    func start(beaconsProvider: @escaping () -> [DetectedBeacon], interval: TimeInterval) {
        self.beaconsProvider = beaconsProvider
        self.interval = interval
        stopTimer()

        // Announce immediately, then repeat
        announce()
        announceTimer = Timer.scheduledTimer(withTimeInterval: interval, repeats: true) { [weak self] _ in
            Task { @MainActor [weak self] in
                self?.announce()
            }
        }
    }

    func stop() {
        stopTimer()
        synthesizer.stopSpeaking(at: .immediate)
    }

    // MARK: - Private

    private func stopTimer() {
        announceTimer?.invalidate()
        announceTimer = nil
    }

    private func announce() {
        guard let beacons = beaconsProvider?(), let strongest = beacons.first else {
            playNoSignalSound()
            return
        }

        let level = SignalLevel.from(normalizedRSSI: strongest.normalizedRSSI)
        let text = String(localized: "voice.announce \(strongest.minor) \(level.localizedName())")
        speak(text)
    }

    private func playNoSignalSound() {
        if synthesizer.isSpeaking {
            synthesizer.stopSpeaking(at: .immediate)
        }
        guard let url = Bundle.main.url(forResource: "no_signal", withExtension: "wav") else { return }
        do {
            try AVAudioSession.sharedInstance().setCategory(.playback, mode: .default)
            try AVAudioSession.sharedInstance().setActive(true)
            noSignalPlayer = try AVAudioPlayer(contentsOf: url)
            noSignalPlayer?.play()
        } catch {
            print("[VoiceGuidance] Audio playback error: \(error)")
        }
    }

    private func speak(_ text: String) {
        if synthesizer.isSpeaking {
            synthesizer.stopSpeaking(at: .immediate)
        }

        let utterance = AVSpeechUtterance(string: text)
        utterance.rate = AVSpeechUtteranceDefaultSpeechRate
        utterance.pitchMultiplier = 1.0
        utterance.volume = 1.0

        // Select language based on current locale
        let langCode = Locale.current.language.languageCode?.identifier ?? "en"
        if langCode == "ja" {
            utterance.voice = AVSpeechSynthesisVoice(language: "ja-JP")
        } else {
            utterance.voice = AVSpeechSynthesisVoice(language: "en-US")
        }

        synthesizer.speak(utterance)
    }
}
