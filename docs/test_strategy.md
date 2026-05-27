# Test Strategy

## Test Pyramid

| Level | Tool | Count | Purpose |
|-------|------|-------|---------|
| Unit Tests | GoogleTest (C++) | 74 | Individual component logic |
| Integration Tests | pytest (Python) | 6+ | Cross-module interactions |
| Scenario Tests | pytest + YAML | 4+ | End-to-end user workflows |
| Negative Tests | Both | 12+ | Error handling and input validation |

## Test Categories

### 1. Smoke Tests
- Verify basic connectivity
- Get/reset state
- Confirm server is responsive

### 2. Unit Tests (C++)
- **EventBus**: Subscribe/publish, exception safety, ring buffer
- **StateManager**: Validation, serialization, reset, signals
- **MediaManager**: State transitions, volume boundaries
- **BluetoothManager**: Connection rules, call lifecycle
- **InputValidator**: All error codes, boundary values

### 3. Integration Tests (Python)
- Media playback flow (play → pause → stop)
- Bluetooth connect/disconnect
- Incoming call pauses media
- Call lifecycle (ring → accept → end)
- Navigation alert during call (priority rule)
- Telemetry update

### 4. Negative Tests
- Unknown command
- Malformed JSON
- Missing action/params fields
- Wrong field types
- Values out of range (volume, speed, temperature)
- Invalid state transitions (pause when stopped)
- Call without Bluetooth connection

### 5. Regression Tests
- YAML scenario suite covering all major flows
- Parametrized pytest execution across all scenarios
- Automated report generation

### 6. State Transition Tests
- Media: Stopped ↔ Playing ↔ Paused
- Bluetooth: Disconnected → Pairing → Connected
- Call: Idle → Ringing → Active → Idle
- Navigation: Idle ↔ AlertActive

### 7. Fault Injection Tests
- Duplicate command IDs
- Command timeout scenarios
- Connection drop and reconnect

## Coverage Goals

- All state machine transitions (valid + invalid)
- All command actions (happy path + error path)
- All validation rules (boundary values)
- All error codes exercised
- Cross-module interaction rules verified

## Reporting

Reports generated in `reports/` directory:
- `latest_report.json` — Machine-readable results
- `latest_report.md` — Human-readable summary
- `failures/*.json` — Individual failure replay files
