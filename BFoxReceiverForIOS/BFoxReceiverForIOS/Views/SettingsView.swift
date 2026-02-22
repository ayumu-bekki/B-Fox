// SettingsView.swift
// BFox iBeacon Receiver

import SwiftUI

struct SettingsView: View {

    @AppStorage(Constants.majorKey)    private var major: Int    = Constants.defaultMajor
    @AppStorage(Constants.uuidKey)     private var uuidString: String = Constants.defaultBFoxProximityUUID
    @AppStorage(Constants.showRSSIKey) private var showRSSI: Bool = false

    /// TextField の編集中バッファ。フォーカスが外れたときにバリデートして uuidString へ書き込む。
    @State private var uuidDraft: String = ""
    @State private var uuidError: Bool = false

    private let majorValues = Array(0...9)

    private var appVersion: String {
        let version = Bundle.main.infoDictionary?["CFBundleShortVersionString"] as? String ?? "–"
        return "\(version)"
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

                    // UUID 入力フィールド
                    VStack(alignment: .leading, spacing: 4) {
                        HStack {
                            Text(String(localized: "settings.uuid.label"))
                                .font(.subheadline)
                                .foregroundStyle(.secondary)
                            Spacer()
                            if uuidString != Constants.defaultBFoxProximityUUID {
                                Button {
                                    uuidDraft = Constants.defaultBFoxProximityUUID
                                    commitUUID()
                                } label: {
                                    Label(String(localized: "settings.uuid.reset"), systemImage: "arrow.counterclockwise")
                                        .font(.caption)
                                }
                                .buttonStyle(.borderless)
                            }
                        }
                        TextField(String(localized: "settings.uuid.placeholder"), text: $uuidDraft)
                            .font(.system(size: 13, design: .monospaced))
                            .autocorrectionDisabled()
                            .textInputAutocapitalization(.characters)
                            .submitLabel(.done)
                            .onSubmit { commitUUID() }
                            .onChange(of: uuidDraft) { _, _ in uuidError = false }
                        if uuidError {
                            Text(String(localized: "settings.uuid.error"))
                                .font(.caption)
                                .foregroundStyle(.red)
                        }
                    }
                    .padding(.vertical, 4)
                    // フォーカスが外れたら確定
                    .onDisappear { commitUUID() }
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
                } header: {
                    Text(String(localized: "settings.section.info"))
                }
            }
            .navigationTitle(String(localized: "tab.settings"))
            .navigationBarTitleDisplayMode(.inline)
            .onAppear { uuidDraft = uuidString }
            // QR スキャン等で uuidString が外部から変わったとき、ドラフトも追従する
            .onChange(of: uuidString) { _, newValue in
                if uuidDraft != newValue { uuidDraft = newValue }
            }
        }
    }

    // MARK: - Private

    private func commitUUID() {
        let trimmed = uuidDraft.trimmingCharacters(in: .whitespaces)
        guard let _ = UUID(uuidString: trimmed) else {
            uuidError = true
            return
        }
        uuidString = trimmed.uppercased()
        uuidError = false
    }
}

// MARK: - Preview

#Preview {
    SettingsView()
}
