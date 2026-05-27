# AI Agent Prompt: Automotive Infotainment Interface Simulator + Test Automation Framework

## Role
You are a senior C++/Python software developer building a resume-grade project for an interface development and testing internship. Implement the project directly. Minimize text output. No acknowledgments. No filler. No motivational comments. No long explanations. Output only concise implementation summaries, file paths changed, commands to run, and blockers if any.

## Project Objective
Build a software-only automotive infotainment interface simulator in C++ with a Python automation and regression testing framework. The project must demonstrate C++ interface development, Python automation scripting, debugging, OOP, state machines, data structures, unit testing, integration testing, negative testing, regression testing, structured logging, and failure replay.

## Required Tech Stack
Use exactly these unless a compatibility issue is found:

- C++20
- Qt 6 Widgets for desktop UI
- CMake 3.22+
- GoogleTest for C++ unit tests
- nlohmann/json for JSON handling
- Python 3.11+
- pytest for Python tests
- PyYAML for YAML scenarios
- jsonschema for schema validation
- Pillow for optional screenshot comparison
- GitHub Actions for CI
- Linux-first development environment

## Application Scope
Create a C++ Qt desktop app that simulates an automotive infotainment system. It must include the following functional modules:

1. Media control
2. Bluetooth connection and call overlay
3. Navigation alerts
4. Vehicle telemetry display
5. User settings
6. Notification queue
7. Central event bus
8. State manager
9. Structured logger
10. TCP JSON command interface for automation

The Python framework must control the C++ app through a local TCP JSON interface, execute YAML-defined scenarios, validate expected states, collect logs, classify failures, and generate test reports.

## Repository Structure
Create this structure:

```text
infotainment-simulator/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── .github/
│   └── workflows/
│       └── ci.yml
├── docs/
│   ├── architecture.md
│   ├── test_strategy.md
│   └── state_machines.md
├── app/
│   ├── CMakeLists.txt
│   ├── main.cpp
│   ├── ui/
│   │   ├── MainWindow.h
│   │   ├── MainWindow.cpp
│   │   ├── DashboardWidget.h
│   │   ├── DashboardWidget.cpp
│   │   ├── MediaWidget.h
│   │   ├── MediaWidget.cpp
│   │   ├── BluetoothWidget.h
│   │   ├── BluetoothWidget.cpp
│   │   ├── NavigationWidget.h
│   │   ├── NavigationWidget.cpp
│   │   ├── TelemetryWidget.h
│   │   ├── TelemetryWidget.cpp
│   │   ├── SettingsWidget.h
│   │   └── SettingsWidget.cpp
│   └── resources/
├── src/
│   ├── core/
│   │   ├── Event.h
│   │   ├── EventBus.h
│   │   ├── EventBus.cpp
│   │   ├── StateManager.h
│   │   ├── StateManager.cpp
│   │   ├── AppState.h
│   │   ├── Logger.h
│   │   ├── Logger.cpp
│   │   ├── CommandServer.h
│   │   └── CommandServer.cpp
│   ├── modules/
│   │   ├── MediaManager.h
│   │   ├── MediaManager.cpp
│   │   ├── BluetoothManager.h
│   │   ├── BluetoothManager.cpp
│   │   ├── NavigationManager.h
│   │   ├── NavigationManager.cpp
│   │   ├── TelemetryManager.h
│   │   ├── TelemetryManager.cpp
│   │   ├── SettingsManager.h
│   │   ├── SettingsManager.cpp
│   │   ├── NotificationManager.h
│   │   └── NotificationManager.cpp
│   └── validation/
│       ├── InputValidator.h
│       └── InputValidator.cpp
├── tests/
│   ├── cpp/
│   │   ├── CMakeLists.txt
│   │   ├── test_media_manager.cpp
│   │   ├── test_bluetooth_manager.cpp
│   │   ├── test_state_manager.cpp
│   │   ├── test_event_bus.cpp
│   │   └── test_input_validator.cpp
│   └── python/
│       ├── requirements.txt
│       ├── pytest.ini
│       ├── conftest.py
│       ├── framework/
│       │   ├── app_client.py
│       │   ├── scenario_loader.py
│       │   ├── scenario_runner.py
│       │   ├── validators.py
│       │   ├── report_generator.py
│       │   ├── log_analyzer.py
│       │   └── failure_replay.py
│       ├── scenarios/
│       │   ├── media_playback.yaml
│       │   ├── incoming_call_pauses_media.yaml
│       │   ├── navigation_alert_priority.yaml
│       │   ├── invalid_inputs.yaml
│       │   └── regression_suite.yaml
│       └── test_scenarios.py
├── schemas/
│   ├── command.schema.json
│   ├── response.schema.json
│   └── scenario.schema.json
├── logs/
│   └── .gitkeep
└── reports/
    └── .gitkeep
```

## C++ Architecture Requirements
Use OOP. Keep classes small and focused. Do not create god classes. Use clear ownership. Prefer smart pointers where needed. Avoid global mutable state except controlled singleton-free dependency injection through constructors.

### Core Types
Define these event types:

```cpp
enum class EventType {
    MediaPlayRequested,
    MediaPauseRequested,
    MediaStopRequested,
    MediaNextRequested,
    MediaPreviousRequested,
    VolumeChanged,
    BluetoothDeviceConnected,
    BluetoothDeviceDisconnected,
    IncomingCallReceived,
    CallAccepted,
    CallRejected,
    CallEnded,
    NavigationAlertStarted,
    NavigationAlertEnded,
    VehicleSpeedChanged,
    FuelLevelChanged,
    EngineTemperatureChanged,
    SettingChanged,
    NotificationQueued,
    NotificationDisplayed,
    ErrorRaised
};
```

Define these state enums:

```cpp
enum class MediaState { Stopped, Playing, Paused };
enum class BluetoothState { Disconnected, Pairing, Connected };
enum class CallState { Idle, Ringing, Active };
enum class NavigationState { Idle, AlertActive };
enum class ActiveScreen { Dashboard, Media, Bluetooth, Navigation, Telemetry, Settings, CallOverlay, ErrorModal };
```

### EventBus
Implement publish/subscribe event routing.

Requirements:
- Allow modules to subscribe to `EventType`.
- Dispatch events synchronously by default.
- Prevent crashes if one handler throws; log the error and continue dispatching.
- Store recent 100 events in an in-memory ring buffer.
- Expose recent events through command interface.

### StateManager
Maintain full application state.

State must include:

```json
{
  "media_state": "playing|paused|stopped",
  "track": "string",
  "volume": 0,
  "bluetooth_state": "connected|pairing|disconnected",
  "connected_device": "string|null",
  "call_state": "idle|ringing|active",
  "navigation_state": "idle|alert_active",
  "active_screen": "dashboard|media|bluetooth|navigation|telemetry|settings|call_overlay|error_modal",
  "vehicle_speed": 0,
  "fuel_level": 0,
  "engine_temperature": 0,
  "notifications_pending": 0,
  "last_error": null
}
```

Rules:
- Volume range: 0 to 100.
- Vehicle speed range: 0 to 240.
- Fuel level range: 0 to 100.
- Engine temperature range: -20 to 140.
- Incoming call must pause playing media.
- Navigation alert must not override call overlay.
- Low-priority notifications must queue during active calls.
- Invalid commands must not mutate state.

### Managers
Implement these classes:

- `MediaManager`
- `BluetoothManager`
- `NavigationManager`
- `TelemetryManager`
- `SettingsManager`
- `NotificationManager`

Each manager must:
- Receive commands through public methods.
- Validate inputs.
- Publish events.
- Update state only through `StateManager`.
- Log state transitions.
- Return structured success/error results.

### TCP JSON Command Interface
Implement `CommandServer` in C++.

Requirements:
- Bind to `127.0.0.1:5555` by default.
- Accept newline-delimited JSON commands.
- Return newline-delimited JSON responses.
- Support one client at minimum; multi-client optional.
- Must not block Qt UI event loop.
- Use a worker thread or Qt networking async APIs.

Commands:

```json
{"id":"1","action":"play_media","params":{"track":"song.mp3"}}
{"id":"2","action":"pause_media","params":{}}
{"id":"3","action":"connect_bluetooth","params":{"device":"Pixel 7"}}
{"id":"4","action":"incoming_call","params":{"caller":"+919999999999"}}
{"id":"5","action":"start_navigation_alert","params":{"message":"Turn left in 200m","priority":"high"}}
{"id":"6","action":"update_telemetry","params":{"speed":60,"fuel":72,"temperature":90}}
{"id":"7","action":"get_state","params":{}}
{"id":"8","action":"get_recent_events","params":{}}
{"id":"9","action":"reset_state","params":{}}
```

Response format:

```json
{
  "id": "1",
  "status": "ok|error",
  "error_code": null,
  "message": "string",
  "state": {}
}
```

Error codes:

- `INVALID_JSON`
- `UNKNOWN_ACTION`
- `MISSING_FIELD`
- `INVALID_FIELD_TYPE`
- `VALUE_OUT_OF_RANGE`
- `INVALID_STATE_TRANSITION`
- `INTERNAL_ERROR`

## UI Requirements
Create a Qt desktop UI with:

- Main dashboard
- Media section
- Bluetooth section
- Navigation alert section
- Telemetry section
- Settings section
- Activity log panel
- Error display area

UI must update from `StateManager` changes. UI should be usable manually but primarily automation-driven.

## Logging Requirements
Use structured JSON Lines logs.

File path:

```text
logs/infotainment_events.jsonl
```

Each log line:

```json
{
  "timestamp":"2026-01-01T10:30:00.000Z",
  "level":"INFO|WARN|ERROR",
  "component":"MediaManager",
  "event_type":"MediaPlayRequested",
  "message":"Media started",
  "before_state":{},
  "after_state":{},
  "correlation_id":"uuid"
}
```

## Python Automation Framework
Build a pytest-based framework that launches the app, connects to TCP command server, runs YAML scenarios, validates responses, validates final state, analyzes logs, and writes reports.

### YAML Scenario Format

```yaml
test_name: incoming_call_should_pause_media
description: Incoming call pauses active media and displays call overlay.
preconditions:
  reset_state: true
steps:
  - action: play_media
    params:
      track: test_track.mp3
    expect:
      status: ok
      state:
        media_state: playing
  - action: connect_bluetooth
    params:
      device: Pixel 7
    expect:
      status: ok
      state:
        bluetooth_state: connected
  - action: incoming_call
    params:
      caller: "+919999999999"
    expect:
      status: ok
      state:
        media_state: paused
        call_state: ringing
        active_screen: call_overlay
final_expect:
  state:
    media_state: paused
    call_state: ringing
    active_screen: call_overlay
```

### Test Categories
Implement these:

1. Smoke tests
2. Unit tests
3. Integration tests
4. Negative tests
5. Regression tests
6. Fault-injection tests
7. State-transition tests

### Required Python Commands
Support:

```bash
python -m pytest tests/python
python tests/python/framework/scenario_runner.py --scenario tests/python/scenarios/media_playback.yaml
python tests/python/framework/scenario_runner.py --suite regression
python tests/python/framework/failure_replay.py --failure reports/failures/<file>.json
```

## C++ Unit Testing
Use GoogleTest. Cover:

- Media state transitions
- Bluetooth connection rules
- Incoming call behavior
- Navigation alert priority rules
- Input validation
- EventBus publish/subscribe
- StateManager reset and serialization
- Invalid transition rejection

## Negative Test Cases
Include tests for:

- Unknown command
- Malformed JSON
- Missing action
- Missing params
- Wrong field type
- Volume below 0
- Volume above 100
- Speed above 240
- Incoming call without Bluetooth connection
- Navigation alert during active call
- Duplicate command IDs
- Command timeout

## Report Requirements
Generate JSON and Markdown reports.

Paths:

```text
reports/latest_report.json
reports/latest_report.md
reports/failures/
```

Report must include:

- Total tests
- Passed
- Failed
- Skipped
- Average response latency
- Slowest command
- Failure category counts
- Failed scenario names
- Failure replay file paths

## CI Requirements
Create GitHub Actions workflow:

- Install Qt dependencies
- Configure CMake
- Build app
- Run GoogleTest
- Install Python dependencies
- Run pytest scenarios in headless mode if possible

## Non-Functional Requirements
- Command response latency under 100 ms for normal commands.
- State serialization under 10 ms.
- No crash on malformed input.
- No state mutation on invalid commands.
- Deterministic test results.
- Clear separation between UI, business logic, and automation interface.
- Avoid unnecessary AI/ML features.
- Keep README recruiter-friendly.

## Implementation Order
Follow this order exactly:

1. Create repository scaffold.
2. Configure CMake, Qt, GoogleTest.
3. Implement core state models.
4. Implement Logger.
5. Implement EventBus.
6. Implement StateManager.
7. Implement managers.
8. Implement TCP JSON CommandServer.
9. Implement minimal Qt UI.
10. Add C++ unit tests.
11. Add Python automation framework.
12. Add YAML scenarios.
13. Add regression reporting.
14. Add failure replay.
15. Add CI.
16. Write README and docs.

## Agent Output Rules
- Do not acknowledge instructions.
- Do not say “sure”, “okay”, or similar filler.
- Do not produce long prose.
- For each response, output only:
  - Files created/modified
  - Key implementation details
  - Commands to run
  - Errors/blockers
- Prefer code changes over explanations.
- Do not skip tests.
- Do not invent external paid services.
- Ask questions only if implementation is blocked.
