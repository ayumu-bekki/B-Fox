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
    private var simulatorTimer: Timer?

    // MARK: - Init

    override init() {
        super.init()
        locationManager.delegate = self
        locationManager.allowsBackgroundLocationUpdates = false
        
        #if targetEnvironment(simulator)
        startSimulatorMock()
        #endif
    }

    // MARK: - Public API

    func startScanning(major: Int, uuid: UUID = Constants.beaconUUID) {
        currentMajor = major
        currentUUID = uuid

        #if targetEnvironment(simulator)
        isScanning = true
        return
        #endif

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
        #if targetEnvironment(simulator)
        isScanning = false
        return
        #endif
        stopRanging()
    }

    func updateMajor(_ major: Int) {
        let wasScanning = isScanning
        if wasScanning { stopRanging() }
        currentMajor = major
        
        #if targetEnvironment(simulator)
        if wasScanning { isScanning = true }
        return
        #endif
        
        if wasScanning { beginRanging() }
    }

    func updateBeaconTarget(uuid: UUID, major: Int) {
        let wasScanning = isScanning
        if wasScanning { stopRanging() }
        currentUUID = uuid
        currentMajor = major
        beacons = []
        
        #if targetEnvironment(simulator)
        if wasScanning { isScanning = true }
        return
        #endif
        
        if wasScanning { beginRanging() }
    }

    // MARK: - Private Helpers

    private func startSimulatorMock() {
        simulatorTimer?.invalidate()
        simulatorTimer = Timer.scheduledTimer(withTimeInterval: 1.2, repeats: true) { [weak self] _ in
            Task { @MainActor [weak self] in
                guard let self = self, self.isScanning else { return }
                self.updateSimulatorBeacons()
            }
        }
    }

    private func updateSimulatorBeacons() {
        let now = Date()
        // 「1番」と「3番」のマイナーIDを持つダミーデータを作成
        let mockData: [(id: Int, rssi: Int)] = [
            (1, -55), // 比較的近い
            (3, -78)  // 少し遠い
        ]
        
        var updated = self.beacons

        for data in mockData {
            let proximity: CLProximity = {
                if data.rssi > -60 { return .immediate }
                if data.rssi > -80 { return .near }
                return .far
            }()

            if let idx = updated.firstIndex(where: { $0.id == data.id }) {
                updated[idx].rssi = data.rssi + Int.random(in: -2...2) // 少し変動させる
                updated[idx].proximity = proximity
                updated[idx].lastSeen = now
            } else {
                updated.append(DetectedBeacon(
                    id: data.id,
                    minor: data.id,
                    rssi: data.rssi,
                    proximity: proximity,
                    lastSeen: now
                ))
            }
        }

        self.beacons = updated.sorted { $0.rssi > $1.rssi }
    }

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
