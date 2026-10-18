# Edge-AI Smart EV Charging Station

An ESP32-based, three-bay EV charging station prototype built with PlatformIO and the Arduino framework. Each bay performs local sensor sampling and embedded model inference, publishes telemetry to ThingsBoard over MQTT, accepts remote control commands, and applies configurable load-management rules.

> **Prototype notice:** This repository is for learning, simulation, and prototyping. It is not certified charging-control or electrical-safety software. Do not connect it to real EV charging hardware or mains power without qualified engineering review and appropriate protection systems.

## Highlights

- **Multi-bay architecture:** Three independent PlatformIO projects (`Bay1`, `Bay2`, and `Bay3`) for ESP32-based charging bays.
- **Edge inference:** Embedded models estimate arrival probability and charging-session duration on the ESP32.
- **IoT telemetry:** MQTT publishing to ThingsBoard for bay status, voltage, current, power, temperature, delivered energy, predictions, and load decisions.
- **Remote control:** ThingsBoard RPC handlers for relay state, throttle level, and switching back to automatic mode.
- **Load-management logic:** Configurable overload thresholds, peak-hour behavior, throttling, and manual overrides.
- **Sensor and status simulation:** Wokwi diagrams with an ESP32, sensor inputs, buttons, relay, and indicator LEDs.
- **Modular C++ code:** Separate modules for networking, peripherals, telemetry, attributes, RPC, state, inference, and optimization.

## Technology Stack

- ESP32 DevKit (`esp32doit-devkit-v1`)
- C++ with the Arduino framework
- PlatformIO
- MQTT with PubSubClient
- ThingsBoard device telemetry, shared attributes, and RPC
- ArduinoJson and DHT sensor library
- Wokwi circuit diagrams

## Repository Layout

```text
Edge-AI-Smart-EV-Charging-Station/
├── Bay1/
│   ├── include/       # Configuration and module headers
│   ├── src/           # ESP32 application and subsystem implementations
│   ├── test/          # Wokwi test/simulation files
│   ├── diagram.json   # Wokwi circuit diagram
│   ├── platformio.ini # PlatformIO environment and dependencies
│   └── wokwi.toml
├── Bay2/              # Independent project for charging bay 2
├── Bay3/              # Independent project for charging bay 3
├── .gitignore
└── README.md
```

## Getting Started

### 1. Install the tools

Install [Visual Studio Code](https://code.visualstudio.com/) and the PlatformIO IDE extension, or use the PlatformIO Core CLI. PlatformIO will install the ESP32 platform and the libraries listed in each bay's `platformio.ini`.

### 2. Configure the ThingsBoard device token

Each bay has its own settings file at `BayX/src/config.cpp`. The `TB_TOKEN` value has intentionally been replaced with a placeholder before publication. To connect a bay to your own ThingsBoard device, set that value locally to the device's access token. Do not commit real access tokens or Wi-Fi passwords to a public repository. Use separate device tokens for separate bays.

The default Wi-Fi settings are for the Wokwi simulator (`Wokwi-GUEST` with an empty password). Change them only when using a different network.

### 3. Build a bay

Open one of the `Bay1`, `Bay2`, or `Bay3` folders as a PlatformIO project. Select the `esp32doit-devkit-v1` environment and run **Build** from PlatformIO. Repeat for each bay as needed.

The equivalent PlatformIO Core command, run from a bay's directory, is:

```bash
pio run
```

To upload to a connected board:

```bash
pio run --target upload
```

### 4. Simulate

Each bay contains a `diagram.json` file for Wokwi. Build the firmware first; Wokwi's configuration refers to generated files in `.pio/build/`, which are intentionally not version-controlled.

## Main Modules

- `main.cpp` — initializes peripherals and networking; coordinates periodic sensing, inference, optimization, and telemetry.
- `Peripherals.cpp` — samples analog inputs and temperature, tracks charging sessions and energy, and handles plug-in/plug-out controls.
- `network.cpp` — manages Wi-Fi/MQTT connectivity, ThingsBoard subscriptions, and incoming messages.
- `telemetry.cpp` — serializes and publishes bay measurements and decisions.
- `attributes.cpp` — loads shared operating parameters from ThingsBoard.
- `rpc.cpp` — handles remote relay, throttle, and automatic-mode commands.
- `edge_ai.cpp` and `model.h` — performs inference using embedded model logic.
- `optimization.cpp` — applies the bay's configurable load-management decisions.

## Notes

- `.pio/` contains generated dependencies, object files, and firmware build output. These are excluded to keep the repository small; PlatformIO regenerates them during a build.
- The three bay folders are independent projects and contain separate per-bay configuration.
- Before using real hardware, verify sensor calibration, relay behavior, fail-safe states, electrical isolation, and the charging system's safety requirements.
