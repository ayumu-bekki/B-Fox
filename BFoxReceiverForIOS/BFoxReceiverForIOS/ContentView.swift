// ContentView.swift
// BFox iBeacon Receiver

import SwiftUI
import CoreLocation

private enum Tab { case radar, qr, settings }

struct ContentView: View {

    @StateObject private var scanner = BeaconScanner()
    @AppStorage(Constants.majorKey)    private var major: Int    = Constants.defaultMajor
    @AppStorage(Constants.uuidKey)     private var uuidString: String = Constants.defaultBFoxProximityUUID
    @AppStorage(Constants.showRSSIKey) private var showRSSI: Bool = false

    @State private var selectedTab: Tab = .radar
    @State private var showQRScanner: Bool = false

    var body: some View {
        TabView(selection: $selectedTab) {
            // MARK: - Tab 1: Radar
            radarTab
                .tabItem {
                    Label(String(localized: "tab.radar"), systemImage: "dot.radiowaves.left.and.right")
                }
                .tag(Tab.radar)

            // MARK: - Tab 2: QR Scan (ボタンとして機能、シートを開く)
            Color.clear
                .tabItem {
                    Label(String(localized: "tab.qr"), systemImage: "qrcode.viewfinder")
                }
                .tag(Tab.qr)

            // MARK: - Tab 3: Settings
            SettingsView()
                .tabItem {
                    Label(String(localized: "tab.settings"), systemImage: "gearshape")
                }
                .tag(Tab.settings)
        }
        .onChange(of: selectedTab) { _, newTab in
            if newTab == .qr {
                showQRScanner = true
                selectedTab = .radar
            }
        }
        .onAppear {
            let uuid = UUID(uuidString: uuidString) ?? Constants.beaconUUID
            scanner.startScanning(major: major, uuid: uuid)
        }
        .onDisappear {
            scanner.stopScanning()
        }
        .onChange(of: major) { _, newValue in
            scanner.updateMajor(newValue)
        }
        .onChange(of: uuidString) { _, newValue in
            guard let uuid = UUID(uuidString: newValue) else { return }
            scanner.updateBeaconTarget(uuid: uuid, major: major)
        }
        .sheet(isPresented: $showQRScanner) {
            QRScannerView { uuid, newMajor in
                uuidString = uuid.uuidString
                major = newMajor
                scanner.updateBeaconTarget(uuid: uuid, major: newMajor)
                showQRScanner = false
            } onDismiss: {
                showQRScanner = false
            }
        }
    }

    // MARK: - Radar Tab

    private var isAuthorizationDenied: Bool {
        scanner.authorizationStatus == .denied || scanner.authorizationStatus == .restricted
    }

    private var radarTab: some View {
        NavigationStack {
            VStack(spacing: 0) {
                if isAuthorizationDenied {
                    // Show the authorization overlay across the full area
                    authorizationOverlay
                        .frame(maxWidth: .infinity, maxHeight: .infinity)
                } else {
                    // --- Radar view ---
                    ZStack {
                        RadarView(beacons: scanner.beacons, showRSSI: showRSSI)
                            .padding(20)
                            .overlay(alignment: .top) {
                                statusBadge
                                    .padding(.top, -6)
                            }
                    }
                    .frame(maxWidth: .infinity)
                    .frame(height: 220)

                    Divider()
                        .padding(.top, -8)
                        .padding(.bottom, 8)

                    // --- List view ---
                    BeaconListView(beacons: scanner.beacons, showRSSI: showRSSI)
                        .frame(maxWidth: .infinity, maxHeight: .infinity)
                }
            }
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .principal) {
                    HStack(spacing: 8) {
                        Image("logo")
                            .resizable()
                            .scaledToFit()
                            .frame(height: 40)
                        Text(String(localized: "nav.title"))
                            .font(.title3).bold()
                    }
                }
            }
        }
    }

    // MARK: - Subviews

    /// Status badge showing scanning state and current Major value.
    private var statusBadge: some View {
        HStack(spacing: 5) {
            Circle()
                .fill(scanner.isScanning ? Color.green : Color.red)
                .frame(width: 7, height: 7)
                .overlay(
                    scanner.isScanning
                        ? Circle().stroke(Color.green.opacity(0.5), lineWidth: 2)
                            .scaleEffect(1.6)
                            .animation(.easeInOut(duration: 1).repeatForever(), value: scanner.isScanning)
                        : nil
                )
            Text(scanner.isScanning ? String(localized: "badge.scanning") : String(localized: "badge.stopped"))
                .font(.system(size: 11, weight: .medium))
                .foregroundStyle(.secondary)
            Text("·")
                .font(.system(size: 11, weight: .light))
                .foregroundStyle(.secondary)
            Text("\(String(localized: "label.major")):\(major)")
                .font(.system(size: 11, weight: .light, design: .monospaced))
                .foregroundStyle(.secondary)
        }
        .padding(.horizontal, 8)
        .padding(.vertical, 4)
        .background(.regularMaterial)
        .clipShape(Capsule())
    }
}

// MARK: - Authorization Overlay

extension ContentView {
    /// Overlay shown when location permission is denied or restricted.
    private var authorizationOverlay: some View {
        VStack(spacing: 16) {
            Image(systemName: "location.slash")
                .font(.system(size: 48))
                .foregroundStyle(.red)
            Text(String(localized: "auth.denied.title"))
                .font(.headline)
            Text(String(localized: "auth.denied.description"))
                .font(.caption)
                .multilineTextAlignment(.center)
                .foregroundStyle(.secondary)
            Button(String(localized: "auth.denied.button")) {
                if let url = URL(string: UIApplication.openSettingsURLString) {
                    UIApplication.shared.open(url)
                }
            }
            .buttonStyle(.borderedProminent)
        }
        .padding(32)
    }
}

// MARK: - Preview

#Preview {
    ContentView()
}
