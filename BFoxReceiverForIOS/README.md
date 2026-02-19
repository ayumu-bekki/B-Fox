# B-Fox Receiver for iOS

iBeacon の距離をリアルタイムにグラフィカル表示する iOS アプリです。

> English documentation: [README.md](README.md)

## 概要

B-Fox Receiver は固定の Proximity UUID を持つ iBeacon をスキャンし、検出したビーコンをドットで表示します。**ドットのサイズが電波強度（RSSI）に比例**し、近いほど大きく表示されます。画面下部のリストには RSSI バーと近接距離ラベルが表示されます。

## 機能

- CoreLocation による iBeacon のリアルタイムレンジング
- ドット表示：サイズと透明度が RSSI に連動
- ビーコンごとに個別位相のフローティングアニメーション
- RSSI 降順のビーコンリスト（近接ラベル・バー付き）
- ビーコン未検出時のスキャン中インジケーター（アニメーションドット）
- Major 値セレクター（0〜9）でビーコングループを絞り込み
- RSSI の dBm 値をドットおよびリスト行に表示するオプション
- 位置情報権限が拒否・制限された場合の全画面プロンプト表示
- 英語 / 日本語ローカライズ対応

## 動作要件

- iOS 18.6 以降
- Xcode 16 以降
- 実機（iBeacon のレンジングはシミュレーターでは動作しません）

## セットアップ

### 1. Team ID を設定

サンプルファイルをコピーして自分の Apple Developer Team ID を設定します。

```bash
cp BFoxReceiverForIOS/Config/Local.xcconfig.sample \
   BFoxReceiverForIOS/Config/Local.xcconfig
```

`Local.xcconfig` を開き、`YOUR_TEAM_ID` を自分の Team ID に書き換えます。

```
DEVELOPMENT_TEAM = YOUR_TEAM_ID
```

Team ID は [developer.apple.com](https://developer.apple.com) の **Membership** ページ、または **Xcode → Settings → Accounts** で確認できます。

### 2. Xcode で xcconfig を割り当て

1. `BFoxReceiverForIOS.xcodeproj` を Xcode で開きます。
2. Project Navigator でプロジェクトルートを選択し、**Info** タブを開きます。
3. **Configurations** セクションで、`BFoxReceiverForIOS` ターゲットの **Debug** と **Release** の両方に **Base** を割り当てます。

### 3. ビルド・実行

接続した実機を選択して **Run**（⌘R）を押します。

## プロジェクト構成

```
BFoxReceiverForIOS/
├── Models/
│   ├── BeaconModel.swift       # DetectedBeacon 構造体
│   └── Constants.swift         # UUID・RSSI 閾値・カラー定義
├── Services/
│   └── BeaconScanner.swift     # CLLocationManager ラッパー（@MainActor）
├── Views/
│   ├── RadarView.swift         # ドットベースのビジュアル表示
│   ├── BeaconListView.swift    # RSSI 降順リスト（バーインジケーター付き）
│   └── SettingsView.swift      # Major セレクターと表示オプション
├── Config/
│   ├── Base.xcconfig           # 共通ビルド設定（git 管理対象）
│   ├── Local.xcconfig          # 個人設定（Team ID など、git 除外）
│   └── Local.xcconfig.sample   # Local.xcconfig のテンプレート
├── ContentView.swift           # TabView を持つルートビュー
├── BFoxReceiverForIOSApp.swift # アプリエントリーポイント
└── Localizable.xcstrings       # 英語 / 日本語文字列
```

## ビーコン設定

| 設定項目 | 値 |
|----------|-----|
| Proximity UUID | `C65B2C5D-9E53-46EC-8B8E-54D9E2F21188` |
| Major | 設定画面で選択（0〜9） |
| Minor | ビーコンの識別子・表示ラベルとして使用 |

UUIDはビーコン側でB-Fox用に定義された値となります。

