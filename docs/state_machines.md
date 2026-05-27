# State Machines

## Media State Machine

```mermaid
stateDiagram-v2
    [*] --> Stopped
    Stopped --> Playing : play(track)
    Playing --> Paused : pause()
    Playing --> Stopped : stop()
    Paused --> Playing : play(track)
    Paused --> Stopped : stop()
    Playing --> Playing : play(new_track)
```

**Rules:**
- Volume: 0–100 (validated)
- Track name required for play
- Cannot pause when stopped
- Cannot stop when already stopped

## Bluetooth State Machine

```mermaid
stateDiagram-v2
    [*] --> Disconnected
    Disconnected --> Pairing : connect(device)
    Pairing --> Connected : pairing_complete
    Connected --> Disconnected : disconnect()
```

**Rules:**
- Cannot connect while already connected
- Disconnect ends any active call
- Device name required for connect

## Call State Machine

```mermaid
stateDiagram-v2
    [*] --> Idle
    Idle --> Ringing : incoming_call(caller)
    Ringing --> Active : accept_call()
    Ringing --> Idle : reject_call()
    Active --> Idle : end_call()
    Ringing --> Idle : end_call()
```

**Rules:**
- Requires Bluetooth connected
- Incoming call auto-pauses playing media
- Incoming call switches screen to CallOverlay
- Cannot receive call during active call
- Reject/end returns screen to Dashboard

## Navigation State Machine

```mermaid
stateDiagram-v2
    [*] --> Idle
    Idle --> AlertActive : start_alert(message, priority)
    AlertActive --> Idle : end_alert()
```

**Rules:**
- Alert does NOT override CallOverlay screen
- Message and priority required
- End alert returns screen to Dashboard (if currently on Navigation)

## Screen Priority

```mermaid
stateDiagram-v2
    [*] --> Dashboard
    Dashboard --> Media : switch_screen
    Dashboard --> Bluetooth : switch_screen
    Dashboard --> Navigation : nav_alert (no call)
    Dashboard --> Telemetry : switch_screen
    Dashboard --> Settings : switch_screen
    Dashboard --> CallOverlay : incoming_call
    CallOverlay --> Dashboard : call_ended
    Navigation --> Dashboard : alert_ended
```

**Priority Order (highest to lowest):**
1. CallOverlay (incoming/active call)
2. Navigation (active alert, only when no call)
3. User-selected screen
4. Dashboard (default)
