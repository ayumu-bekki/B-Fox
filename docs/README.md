# BleRDFプロジェクト 技術情報概要

BleRDFは、Bluetooth Low Energy (BLE) を利用し、アマチュア無線方向探知 (ARDF) の遊びを再現・実現するためのオープンソースプロジェクトです。

---

## 主要コンポーネントとハードウェア

### 送受信機メインモジュール

Seeed Studio XIAO ESP32C6

- BLE による低消費電力での無線通信を実現。
- 外部アンテナ利用可能 技適 (技術基準適合証明) 取得済み
- リポバッテリー対応  携帯利用に適した電源
- 小型フットプリント デバイスの小型化・軽量化

### 受信機インターフェース
キャラクタ液晶表示器を採用
LCDモジュール 16×2行 AQM1602Y-RN-GBW 秋月電子通商取扱([https://akizukidenshi.com/catalog/g/g111916/](https://akizukidenshi.com/catalog/g/g111916/))

---

## ソフトウェアと通信プロトコル

### 開発環境
esp-idf 5.5.1
Espressif IoT Development Framework ([https://github.com/espressif/esp-idf](https://github.com/espressif/esp-idf))

### 通信プロトコル
iBeacon (BLE) プロトコルを利用

GAPを利用し、ビーコン情報を定期的にブロードキャスト
GATTを利用し、送信機の設定変更やバッテリー電圧の取得
スマートフォンアプリ LightBlue ([https://punchthrough.com/lightblue/]https://punchthrough.com/lightblue/)  などで受信可能（この場合、iBeaconではなくGAPによるデバイス名を利用）

---

## 3. ハードウェア製造

送受信機ともにケース・基板 (PCB) のデータを用意しています。
PCBWay ([https://www.pcbway.com](https://www.pcbway.com)) などのサービスを利用して基板・ケースの出力が可能。

---

## 4. 必要な主要パーツ一覧

リファレンス実装に必要な主要パーツ一覧

### 送信機

| 項目 | 備考 | 個数|
| --- | --- | --- |
| Seeed Studio XIAO ESP32C6 |  | 1 |
| リポバッテリー 3000mAh | | 1 |

### 受信機

| 項目 | 備考 | 個数|
| --- | --- | --- |
| Seeed Studio XIAO ESP32C6 |  | 1 |
| 受信機基板 |  | 1 |
| 受信機ケース基盤 |  | 1 |
| LCDモジュール AQM1602Y-RN-GBW | | 1 |
| XIAO用ロッドアンテナ | https://akizukidenshi.com/catalog/g/g117582/ | 1 |
| リポバッテリー 500mAh | | 1 |
| 抵抗 | 220kΩ | 2 |
|  | 1kΩ| 2 |
| XH端子 | 2P縦型 | 2 |
| XH端子付きケーブル | 2P (ESP32C6のバッテリー接続用) | 1 |
| | 2P横型 | 1 |
| コンデンサ | | |
| スライドスイッチ | 基板実装用 | 1 |
| タクトスイッチ | 基板実装用 | 1 |
| ポリスイッチ | 250mA | 1 |
| 分割ロングピンソケット | 9pin | 1 |
| | 7pin(ロープロファイル) | 2 |
| ネジ| | | 
| スペーサー | | |
| 配線用コード | | 適量 |

