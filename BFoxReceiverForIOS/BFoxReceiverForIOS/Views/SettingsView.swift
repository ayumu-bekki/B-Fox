// SettingsView.swift
// BFox iBeacon Receiver

import SwiftUI

struct SettingsView: View {

    @AppStorage(Constants.majorKey)   private var major: Int  = Constants.defaultMajor
    @AppStorage(Constants.showRSSIKey) private var showRSSI: Bool = false

    private let majorValues = Array(0...9)

    private var appVersion: String {
        let version = Bundle.main.infoDictionary?["CFBundleShortVersionString"] as? String ?? "–"
        let build   = Bundle.main.infoDictionary?["CFBundleVersion"] as? String ?? "–"
        return "\(version) (\(build))"
    }

    var body: some View {
        NavigationStack {
            Form {
                // MARK: - Beacon Settings
                Section {
                    VStack(alignment: .leading, spacing: 6) {
                        Text(String(localized: "settings.major.label"))
                            .font(.subheadline)
                            .foregroundStyle(.secondary)
                        Picker(String(localized: "settings.major.label"), selection: $major) {
                            ForEach(majorValues, id: \.self) { value in
                                Text("\(value)").tag(value)
                            }
                        }
                        .pickerStyle(.segmented)
                    }
                    .padding(.vertical, 4)

                    HStack {
                        Image(systemName: "info.circle")
                            .foregroundStyle(.secondary)
                            .font(.caption)
                        Text(String(localized: "settings.major.hint"))
                            .font(.caption)
                            .foregroundStyle(.secondary)
                    }
                } header: {
                    Text(String(localized: "settings.section.beacon"))
                }

                // MARK: - Display Settings
                Section {
                    Toggle(isOn: $showRSSI) {
                        Label {
                            VStack(alignment: .leading, spacing: 2) {
                                Text(String(localized: "settings.rssi.toggle"))
                                Text(String(localized: "settings.rssi.description"))
                                    .font(.caption)
                                    .foregroundStyle(.secondary)
                            }
                        } icon: {
                            Image(systemName: "waveform")
                                .foregroundStyle(Color.accentColor)
                        }
                    }
                } header: {
                    Text(String(localized: "settings.section.display"))
                }

                // MARK: - Info
                Section {
                    LabeledContent(String(localized: "settings.info.version")) {
                        Text(appVersion)
                            .font(.system(size: 11, design: .monospaced))
                            .foregroundStyle(.secondary)
                    }

                    LabeledContent(String(localized: "settings.info.uuid")) {
                        Text(Constants.beaconUUID.uuidString)
                            .font(.system(size: 11, design: .monospaced))
                            .foregroundStyle(.secondary)
                            .multilineTextAlignment(.trailing)
                    }
                } header: {
                    Text(String(localized: "settings.section.info"))
                }
            }
            .navigationTitle(String(localized: "tab.settings"))
            .navigationBarTitleDisplayMode(.inline)
        }
    }
}

// MARK: - Preview

#Preview {
    SettingsView()
}
