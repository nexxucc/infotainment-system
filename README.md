# Automotive Infotainment Interface Simulator

A software-only automotive infotainment system simulator built with **C++20/Qt 6** and a **Python automation testing framework**. Demonstrates interface development, test automation, OOP, state machines, structured logging, and CI/CD practices.

## Features

- **Qt 6 Desktop UI** — Dark-themed dashboard with media, Bluetooth, navigation, telemetry, and settings panels
- **State Machine Engine** — Rigorous state management for media playback, Bluetooth connections, call handling, and navigation alerts
- **TCP JSON Command Interface** — Remote control via newline-delimited JSON over TCP (port 5555)
- **Python Test Automation** — pytest-based framework executing YAML-defined test scenarios
- **Structured Logging** — JSON Lines event log with before/after state snapshots
- **74 C++ Unit Tests** — GoogleTest suite covering all state transitions and validation rules
- **26+ Python Tests** — Smoke, integration, negative, state-transition, and regression tests
- **Failure Replay** — Re-execute failed scenarios from saved failure reports
- **GitHub Actions CI** — Automated build, test, and report pipeline

## Architecture

```
┌─────────────────────────────────────────────────────┐
│                    Qt UI Layer                      │
│ Dashboard │ Media │ BT │ Nav │ Telemetry │ Settings │
├─────────────────────────────────────────────────────┤
│                   StateManager                      │
├──────────┬──────────┬──────────┬────────────────────┤
│  Media   │Bluetooth │   Nav    │   Telemetry        │
│  Manager │ Manager  │ Manager  │   Manager          │
├──────────┴──────────┴──────────┴────────────────────┤
│           EventBus  │  Logger  │  InputValidator    │
├─────────────────────────────────────────────────────┤
│                  TCP CommandServer                  │
├─────────────────────────────────────────────────────┤
│            Python Automation Framework              │
│     scenario_runner │ validators │ report_gen       │
└─────────────────────────────────────────────────────┘
```

## Tech Stack

| Component | Technology |
|-----------|-----------|
| Language | C++20 |
| UI Framework | Qt 6 Widgets |
| Build System | CMake 3.22+ |
| C++ Testing | GoogleTest |
| JSON | nlohmann/json |
| Automation | Python 3.11+ / pytest |
| Scenarios | YAML (PyYAML) |
| CI | GitHub Actions |

## Building

### Prerequisites

- GCC 12+ or Clang 15+ (C++20 support)
- Qt 6 (Widgets + Network)
- CMake 3.22+
- GoogleTest
- nlohmann/json
- Python 3.11+

### Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

### Run

```bash
./build/app/infotainment-simulator
```

The command server starts on `127.0.0.1:5555`. Use `--port <N>` to override.

## Testing

### C++ Unit Tests

```bash
./build/tests/cpp/infotainment-tests
```

### Python Automation Tests

```bash
# Set up venv
python3 -m venv venv
source venv/bin/activate
pip install -r tests/python/requirements.txt

# Start the app (in another terminal or background)
QT_QPA_PLATFORM=offscreen ./build/app/infotainment-simulator &

# Run all tests
cd tests/python
python -m pytest -v

# Run specific scenario
python framework/scenario_runner.py --scenario scenarios/media_playback.yaml

# Run regression suite
python framework/scenario_runner.py --suite regression

# Replay a failure
python framework/failure_replay.py --failure ../../reports/failures/<file>.json
```

## TCP JSON Protocol

### Send a command

```bash
echo '{"id":"1","action":"play_media","params":{"track":"song.mp3"}}' | nc localhost 5555
```

### Available commands

| Action | Required Params |
|--------|----------------|
| `play_media` | `track` (string) |
| `pause_media` | — |
| `stop_media` | — |
| `set_volume` | `volume` (0–100) |
| `connect_bluetooth` | `device` (string) |
| `disconnect_bluetooth` | — |
| `incoming_call` | `caller` (string) |
| `accept_call` | — |
| `reject_call` | — |
| `end_call` | — |
| `start_navigation_alert` | `message`, `priority` |
| `end_navigation_alert` | — |
| `update_telemetry` | `speed` (0–240), `fuel` (0–100), `temperature` (-20–140) |
| `get_state` | — |
| `get_recent_events` | — |
| `reset_state` | — |

## Project Structure

```
├── app/                    # Qt application
│   ├── main.cpp
│   └── ui/                 # Widget implementations
├── src/
│   ├── core/               # EventBus, StateManager, Logger, CommandServer
│   ├── modules/            # Media, Bluetooth, Nav, Telemetry, Settings, Notification managers
│   └── validation/         # Input validation
├── tests/
│   ├── cpp/                # GoogleTest unit tests
│   └── python/             # pytest automation framework
│       ├── framework/      # Client, runner, validators, reports
│       └── scenarios/      # YAML test scenarios
├── schemas/                # JSON schemas
├── docs/                   # Architecture, test strategy, state machines
├── logs/                   # Structured event logs
└── reports/                # Test reports and failure replays
```
