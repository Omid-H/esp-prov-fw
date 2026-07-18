<!-- Gradient Divider -->
<img src="https://user-images.githubusercontent.com/73097560/115834477-dbab4500-a447-11eb-908a-139a6edaec5c.gif">

<!-- Dynamic Header -->
<img height=350 alt="Repo Banner - esp-prov-fw" src="https://capsule-render.vercel.app/api?type=waving&color=0:4F46E5,100:06B6D4&height=300&section=header&text=🚀esp-prov-fw&fontSize=50&fontColor=ffffff&animation=fadeIn&fontAlignY=38&desc=Unified%20Wi-Fi%20Provisioning%20Firmware%20for%20ESP32&descAlignY=60&descAlign=50"></img>

<!--Title-->
<p align="center">
  <b>Unified Wi-Fi Provisioning Engine – ESP-IDF Ecosystem (v1.0)</b>
  <br> <small> <i>- A production-ready ESP-IDF firmware implementing secure BLE and SoftAP provisioning with physical device recovery mechanisms -</i> </small> <br>
</p> 

---

## 🔧 Introduction
Project **esp-prov-fw** is a clean, production-grade **Wi-Fi provisioning firmware** built on top of the native ESP-IDF `network_provisioning` and `protocomm` components. It handles secure, out-of-the-box wireless credentials configuration, featuring:
- **Dual-Transport Layer:** Support for high-speed BLE (NimBLE-backed) and standalone SoftAP fallback modes.
- **Hardened Security Primitives:** Secure communication channels supporting Curve25519 key-exchange (Sec V1) and SRP6a-based Secure Remote Password protocol (Sec V2).
- **Physical Device Recovery:** Embedded background tasks to execute NVS flash wipes and system resets via on-board button interactions.
- **Asynchronous Telemetry & Feedback:** Visual feedback blinking queues and terminal QR Code generation to assist in fast onboarding.

Ideally suited as a bootstrap configuration layer for IoT sensors, smart home appliances, and edge gateways.

---

## 💻 Software Architecture
Implemented via a high-performance **C/C++ (ESP-IDF 6.0)** runtime utilizing native FreeRTOS tasks:

### Core Execution Modules
- **Asynchronous Button Monitor Task**  
  Background polling loop tracking GPIO 0 input levels. Features software debounce timers to safely distinguish normal clicks from factory reset triggers.
- **Unified Provisioning Orchestrator**  
  Initializes `network_provisioning` manager dynamically. Automatically prints a QR Code to the serial console for swift mobile app scans.
- **Visual Heartbeat Indicator**  
  Dedicated low-priority IO task executing a brief pulse pattern to signal runtime sanity and network status.
- **Secure Key Exchange Layers**  
  Configurable security modules including AES-GCM-128 payload encryption coupled with client validation keys.

### Distributed Network Engine
- **Protocomm Endpoints**  
  Exposes standard `prov-session`, `prov-config`, and custom `custom-data` command routing ports.
- **NVS Partition Wiper**  
  Direct hardware access using `nvs_flash_erase` to erase device configurations and reset the network state safely.
- **Dual Host Bluetooth Stack**  
  Supports optimized NimBLE stack for low-memory environments, as well as the classic Bluedroid host.

> 📢 Based on Espressif Technologies examples.  
> 📚 See the official [ESP-IDF Provisioning Documentation](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/provisioning/index.html) for more information.

---

## 🚀 Key Features
| Operational Vector | Functional Characteristics |
|------------------|--------------------------------------------------------------------------|
| **Transports**   | Ble-GATT provisioning (NimBLE host) • SoftAP Server • Terminal QR Code rendering |
| **Security**     | Sec V1 (Curve25519 + AES) • Sec V2 (SRP6a + SHA256 + AES) • Force Encryption GATT configs |
| **Resilience**   | Independent FreeRTOS system tasks • Isolated flash memory manipulation • Watchdog integration |
| **Diagnostics**  | Verbose serial debug output • custom data endpoints for telemetry checks • NVS status auditing |

---

## 📡 Provisioning Transports & Security
| Sub-System     | Transport Protocol | Encryption Type                           | Validation Mode |
|----------------|-------------------|-------------------------------------------|-------------------|
| **BLE Mode**   | Bluetooth LE GATT | AES-GCM-128 (Sec V1 / V2)                 | SRP6a / Curve25519 Shared Secrets |
| **SoftAP Mode**| HTTP Web Server   | AES-GCM-128 Over HTTP Payload             | SRP6a / Curve25519 Shared Secrets |

---

## 🛡️ Device Recovery & Flash Reset
- **Factory Reset Routine:** Press and hold the BOOT button (GPIO 0) for 3 seconds. The background task will log the countdown, erase the NVS partition (wiping all saved Wi-Fi credentials), and restart the ESP32.
- **Heap Integrity Guard:** Cleans up and turns off the BLE/Bluetooth stack dynamically immediately after successful provisioning to free runtime memory.
- **Heartbeat Status Indicator:** Continuous GPIO pulse ensures that the user can immediately identify if the firmware is running or locked.

## 📱 Companion Android Client
To provision this firmware easily from a mobile device, a custom-built flutter companion application is available:

* 🔗 **Repository Link:** [Omid-H/Provisio](https://github.com/Omid-H/Provisio)
* 🛠️ **Functional Scope:** Supports asynchronous BLE node discovery, camera-based QR code decoding, secure authentication handshakes (Sec V1/V2), and real-time setup verification.

---

## 📜 Version Ledger
| Generation | Deployment Context | Notable Milestones & Iterations |
|----------|---------------|----------------------------------------------------------------------------------|
| **v1.0** | **July 2026** | **Initial release of esp-prov-fw project codebase.** |

---

## 🤝 Contributing

Contributions are welcome and greatly appreciated! Whether you're fixing bugs, improving documentation, optimizing the control algorithms, or proposing new features, your help makes **esp-prov-fw** better.

### Development Workflow
1. Fork the repository.
2. Create a new branch:
```bash
git checkout -b feature/my-awesome-feature
```
3. Commit your changes:
```bash
git commit -m "Add: my awesome feature"
```
4. Push to your fork:
```bash
git push origin feature/my-awesome-feature
```
5. Open a Pull Request describing your changes and their motivation.

---

## 📚 License
Licensed under the terms of the **Apache License 2.0**. For explicit permissions and warranty declarations, review the companion [LICENSE](https://github.com/nariman-z/esp-prov-fw/blob/main/LICENSE) document asset.

Original work Copyright © Espressif Systems.

---

<div align="center">

### Made with ❤️ by [Nariman_Z](https://github.com/nariman-z)  
🌐 [Personal Website](https://nariman-z.pages.dev/)

</div>

<!-- Gradient Divider -->
<img src="https://user-images.githubusercontent.com/73097560/115834477-dbab4500-a447-11eb-908a-139a6edaec5c.gif">

_Last Update: July 18, 2026_
