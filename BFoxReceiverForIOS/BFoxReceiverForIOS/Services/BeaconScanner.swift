// BeaconScanner.swift
// BFox iBeacon Receiver

import Foundation
import CoreLocation
import Combine

/// ObservableObject that manages iBeacon scanning.
@MainActor
final class BeaconScanner: NSObject, ObservableObject {

    // MARK: - Published State

    @Published private(set) var beacons: [DetectedBeacon] = []
    @Published private(set) var authorizationStatus: CLAuthorizationStatus = .notDetermined
    @Published private(set) var isScanning: Bool = false

    // MARK: - Private

    private let locationManager = CLLocationManager()
    private var currentMajor: Int = Constants.defaultMajor
    private var currentUUID: UUID = Constants.beaconUUID
    private let beaconTimeoutInterval: TimeInterval = 2.0
    private var timeoutTimer: Timer?

    // MARK: - Init

    override init() {
        super.init()
        locationManager.delegate = self
        locationManager.allowsBackgroundLocationUpdates = false
    }

    // MARK: - Public API

    func startScanning(major: Int, uuid: UUID = Constants.beaconUUID) {
        currentMajor = major
        currentUUID = uuid

        switch locationManager.authorizationStatus {
        case .authorizedWhenInUse, .authorizedAlways:
            beginRanging()
        case .notDetermined:
            locationManager.requestWhenInUseAuthorization()
        default:
            break
        }
    }

    func stopScanning() {
        stopRanging()
    }

    func updateMajor(_ major: Int) {
        let wasScanning = isScanning
        if wasScanning { stopRanging() }
        currentMajor = major
        if wasScanning { beginRanging() }
    }

    func updateBeaconTarget(uuid: UUID, major: Int) {
        let wasScanning = isScanning
        if wasScanning { stopRanging() }
        currentUUID = uuid
        currentMajor = major
        beacons = []
        if wasScanning { beginRanging() }
    }

    // MARK: - Private Helpers

    private func beginRanging() {
        let constraint = CLBeaconIdentityConstraint(
            uuid: currentUUID,
            major: CLBeaconMajorValue(currentMajor)
        )
        locationManager.startRangingBeacons(satisfying: constraint)
        isScanning = true
        startTimeoutTimer()
    }

    private func stopRanging() {
        let constraint = CLBeaconIdentityConstraint(
            uuid: currentUUID,
            major: CLBeaconMajorValue(currentMajor)
        )
        locationManager.stopRangingBeacons(satisfying: constraint)
        isScanning = false
        timeoutTimer?.invalidate()
        timeoutTimer = nil
    }

    private func startTimeoutTimer() {
        timeoutTimer?.invalidate()
        timeoutTimer = Timer.scheduledTimer(withTimeInterval: 1.0, repeats: true) { [weak self] _ in
            self?.removeTimedOutBeacons()
        }
    }

    private func removeTimedOutBeacons() {
        let now = Date()
        beacons.removeAll { now.timeIntervalSince($0.lastSeen) > beaconTimeoutInterval }
    }
}

// MARK: - CLLocationManagerDelegate

extension BeaconScanner: CLLocationManagerDelegate {

    nonisolated func locationManagerDidChangeAuthorization(_ manager: CLLocationManager) {
        let status = manager.authorizationStatus
        Task { @MainActor in
            authorizationStatus = status
            if (status == .authorizedWhenInUse || status == .authorizedAlways) && !isScanning {
                beginRanging()
            }
        }
    }

    nonisolated func locationManager(
        _ manager: CLLocationManager,
        didRange beacons: [CLBeacon],
        satisfying constraint: CLBeaconIdentityConstraint
    ) {
        let now = Date()
        let updates = beacons.filter { $0.rssi != 0 }

        Task { @MainActor in
            var updated = self.beacons
            for clBeacon in updates {
                let minor = Int(truncating: clBeacon.minor)
                if let idx = updated.firstIndex(where: { $0.id == minor }) {
                    updated[idx].rssi = clBeacon.rssi
                    updated[idx].proximity = clBeacon.proximity
                    updated[idx].lastSeen = now
                } else {
                    updated.append(DetectedBeacon(
                        id: minor,
                        minor: minor,
                        rssi: clBeacon.rssi,
                        proximity: clBeacon.proximity,
                        lastSeen: now
                    ))
                }
            }

            self.beacons = updated.sorted { $0.rssi > $1.rssi }
        }
    }

    nonisolated func locationManager(_ manager: CLLocationManager, didFailWithError error: Error) {
        print("[BeaconScanner] Error: \(error.localizedDescription)")
    }
}
