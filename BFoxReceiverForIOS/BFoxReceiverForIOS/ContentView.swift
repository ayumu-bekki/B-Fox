// ContentView.swift
// BFox iBeacon Receiver

import SwiftUI
import CoreLocation

struct ContentView: View {

    @StateObject private var scanner = BeaconScanner()
    @AppStorage(Constants.majorKey)    private var major: Int   = Constants.defaultMajor
    @AppStorage(Constants.showRSSIKey) private var showRSSI: Bool = false

    var body: some View {
        TabView {
            // MARK: - Tab 1: Radar
            radarTab
                .tabItem {
                    Label(String(localized: "tab.radar"), systemImage: "dot.radiowaves.left.and.right")
                }

            // MARK: - Tab 2: Settings
            SettingsView()
                .tabItem {
                    Label(String(localized: "tab.settings"), systemImage: "gearshape")
                }
        }
        .onAppear {
            scanner.startScanning(major: major)
        }
        .onDisappear {
            scanner.stopScanning()
        }
        .onChange(of: major) { _, newValue in
            scanner.updateMajor(newValue)
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
