// QRScannerView.swift
// BFox iBeacon Receiver

import SwiftUI
import AVFoundation

/// QRコードをスキャンしてビーコンターゲット（UUID + MajorID）を設定するシート。
/// QRコードのフォーマット: BFOX:<UUID>:<MajorID>
struct QRScannerView: View {

    /// スキャン成功時に呼ばれる。uuid と major を返す。
    var onScanned: (UUID, Int) -> Void
    var onDismiss: () -> Void

    @State private var errorMessage: String? = nil
    @State private var scanned: Bool = false
    @State private var showSuccessBanner: Bool = false

    var body: some View {
        NavigationStack {
            ZStack {
                CameraPreviewView(onScanned: handleRawString)
                    .ignoresSafeArea()

                // Viewfinder overlay
                VStack {
                    Spacer()
                    RoundedRectangle(cornerRadius: 12)
                        .strokeBorder(Color.white.opacity(0.8), lineWidth: 2)
                        .frame(width: 240, height: 240)
                        .background(
                            RoundedRectangle(cornerRadius: 12)
                                .fill(Color.white.opacity(0.05))
                        )
                    Text(String(localized: "qr.hint"))
                        .font(.caption)
                        .foregroundStyle(.white)
                        .padding(.top, 12)
                    Spacer()
                }

                if let errorMessage {
                    VStack(spacing: 12) {
                        Image(systemName: "xmark.circle.fill")
                            .font(.system(size: 36))
                            .foregroundStyle(.red)
                        Text(errorMessage)
                            .font(.subheadline)
                            .multilineTextAlignment(.center)
                            .foregroundStyle(.white)
                        Button(String(localized: "qr.retry")) {
                            self.errorMessage = nil
                            scanned = false
                        }
                        .buttonStyle(.borderedProminent)
                    }
                    .padding(24)
                    .background(.ultraThinMaterial)
                    .clipShape(RoundedRectangle(cornerRadius: 16))
                    .padding()
                }

                // 成功バナー
                if showSuccessBanner {
                    VStack {
                        HStack(spacing: 10) {
                            Image(systemName: "checkmark.circle.fill")
                                .foregroundStyle(.green)
                            Text(String(localized: "qr.success"))
                                .font(.subheadline.weight(.medium))
                                .foregroundStyle(.primary)
                        }
                        .padding(.horizontal, 20)
                        .padding(.vertical, 14)
                        .background(.regularMaterial)
                        .clipShape(Capsule())
                        .shadow(color: .black.opacity(0.15), radius: 8, y: 4)
                        .padding(.top, 16)
                        Spacer()
                    }
                    .transition(.move(edge: .top).combined(with: .opacity))
                }
            }
            .navigationTitle(String(localized: "qr.title"))
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .cancellationAction) {
                    Button(String(localized: "qr.cancel"), action: onDismiss)
                }
            }
        }
        .sensoryFeedback(.success, trigger: showSuccessBanner)
    }

    // MARK: - Parse

    private func handleRawString(_ raw: String) {
        guard !scanned else { return }

        let parts = raw.split(separator: ":", maxSplits: 2)
        guard parts.count == 3,
              parts[0].uppercased() == "BFOX",
              let uuid = UUID(uuidString: String(parts[1])),
              let major = Int(parts[2]), major >= 0, major <= 65535
        else {
            scanned = true
            errorMessage = String(localized: "qr.error.invalid")
            return
        }

        scanned = true
        withAnimation(.spring(response: 0.4, dampingFraction: 0.7)) {
            showSuccessBanner = true
        }
        // バナーを見せてからシートを閉じる
        Task {
            try? await Task.sleep(for: .milliseconds(1500))
            onScanned(uuid, major)
        }
    }
}

// MARK: - Camera Preview (UIViewControllerRepresentable)

private struct CameraPreviewView: UIViewControllerRepresentable {

    var onScanned: (String) -> Void

    func makeUIViewController(context: Context) -> CameraViewController {
        let vc = CameraViewController()
        vc.onScanned = onScanned
        return vc
    }

    func updateUIViewController(_ uiViewController: CameraViewController, context: Context) {}
}

// MARK: - CameraViewController

private final class CameraViewController: UIViewController, AVCaptureMetadataOutputObjectsDelegate {

    var onScanned: ((String) -> Void)?

    private var session: AVCaptureSession?
    private var previewLayer: AVCaptureVideoPreviewLayer?

    override func viewDidLoad() {
        super.viewDidLoad()
        view.backgroundColor = .black
        setupSession()
    }

    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        DispatchQueue.global(qos: .userInitiated).async { [weak self] in
            self?.session?.startRunning()
        }
    }

    override func viewDidDisappear(_ animated: Bool) {
        super.viewDidDisappear(animated)
        session?.stopRunning()
    }

    override func viewDidLayoutSubviews() {
        super.viewDidLayoutSubviews()
        previewLayer?.frame = view.bounds
    }

    private func setupSession() {
        let session = AVCaptureSession()

        guard let device = AVCaptureDevice.default(for: .video),
              let input = try? AVCaptureDeviceInput(device: device),
              session.canAddInput(input)
        else { return }

        session.addInput(input)

        let output = AVCaptureMetadataOutput()
        guard session.canAddOutput(output) else { return }
        session.addOutput(output)
        output.setMetadataObjectsDelegate(self, queue: .main)
        output.metadataObjectTypes = [.qr]

        let preview = AVCaptureVideoPreviewLayer(session: session)
        preview.videoGravity = .resizeAspectFill
        preview.frame = view.bounds
        view.layer.addSublayer(preview)
        previewLayer = preview

        self.session = session
    }

    // MARK: - AVCaptureMetadataOutputObjectsDelegate

    func metadataOutput(
        _ output: AVCaptureMetadataOutput,
        didOutput metadataObjects: [AVMetadataObject],
        from connection: AVCaptureConnection
    ) {
        guard let obj = metadataObjects.first as? AVMetadataMachineReadableCodeObject,
              let stringValue = obj.stringValue
        else { return }

        session?.stopRunning()
        onScanned?(stringValue)
    }
}
