"""Pytest test module that runs YAML scenarios and direct command tests."""

import os
import sys
import pytest

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from framework.app_client import AppClient
from framework.scenario_loader import load_scenario, load_all_scenarios
from framework.validators import validate_response, validate_state


SCENARIO_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "scenarios")


# ============================================================
# Smoke Tests
# ============================================================

class TestSmoke:
    """Basic connectivity and state retrieval tests."""

    @pytest.mark.smoke
    def test_get_state(self, client):
        resp = client.send_command("get_state")
        assert resp["status"] == "ok"
        assert "state" in resp

    @pytest.mark.smoke
    def test_reset_state(self, client):
        resp = client.send_command("reset_state")
        assert resp["status"] == "ok"

    @pytest.mark.smoke
    def test_get_recent_events(self, client):
        resp = client.send_command("get_recent_events")
        assert resp["status"] == "ok"


# ============================================================
# Integration Tests
# ============================================================

class TestIntegration:
    """Cross-module integration tests."""

    @pytest.mark.integration
    def test_media_playback_flow(self, client):
        resp = client.send_command("play_media", {"track": "song.mp3"})
        assert resp["status"] == "ok"
        assert resp["state"]["media_state"] == "playing"

        resp = client.send_command("pause_media")
        assert resp["status"] == "ok"
        assert resp["state"]["media_state"] == "paused"

        resp = client.send_command("stop_media")
        assert resp["status"] == "ok"
        assert resp["state"]["media_state"] == "stopped"

    @pytest.mark.integration
    def test_bluetooth_connect_disconnect(self, client):
        resp = client.send_command("connect_bluetooth", {"device": "Pixel 7"})
        assert resp["status"] == "ok"
        assert resp["state"]["bluetooth_state"] == "connected"

        resp = client.send_command("disconnect_bluetooth")
        assert resp["status"] == "ok"
        assert resp["state"]["bluetooth_state"] == "disconnected"

    @pytest.mark.integration
    def test_incoming_call_pauses_media(self, client):
        client.send_command("play_media", {"track": "song.mp3"})
        client.send_command("connect_bluetooth", {"device": "Pixel 7"})

        resp = client.send_command("incoming_call", {"caller": "+919999999999"})
        assert resp["status"] == "ok"
        assert resp["state"]["media_state"] == "paused"
        assert resp["state"]["call_state"] == "ringing"
        assert resp["state"]["active_screen"] == "call_overlay"

    @pytest.mark.integration
    def test_call_lifecycle(self, client):
        client.send_command("connect_bluetooth", {"device": "Pixel 7"})
        client.send_command("incoming_call", {"caller": "+919999999999"})
        
        resp = client.send_command("accept_call")
        assert resp["state"]["call_state"] == "active"

        resp = client.send_command("end_call")
        assert resp["state"]["call_state"] == "idle"

    @pytest.mark.integration
    def test_nav_alert_during_call(self, client):
        """Navigation alert should not override call overlay."""
        client.send_command("connect_bluetooth", {"device": "Pixel 7"})
        client.send_command("incoming_call", {"caller": "+919999999999"})
        client.send_command("accept_call")

        resp = client.send_command("start_navigation_alert", {
            "message": "Turn left in 200m",
            "priority": "high"
        })
        assert resp["status"] == "ok"
        assert resp["state"]["navigation_state"] == "alert_active"
        assert resp["state"]["active_screen"] == "call_overlay"

    @pytest.mark.integration
    def test_telemetry_update(self, client):
        resp = client.send_command("update_telemetry", {
            "speed": 60, "fuel": 72, "temperature": 90
        })
        assert resp["status"] == "ok"
        assert resp["state"]["vehicle_speed"] == 60
        assert resp["state"]["fuel_level"] == 72
        assert resp["state"]["engine_temperature"] == 90


# ============================================================
# Negative Tests
# ============================================================

class TestNegative:
    """Tests for invalid inputs and error handling."""

    @pytest.mark.negative
    def test_unknown_command(self, client):
        resp = client.send_raw('{"id":"1","action":"fly_to_moon","params":{}}')
        assert resp["status"] == "error"

    @pytest.mark.negative
    def test_malformed_json(self, client):
        resp = client.send_raw('{invalid json}')
        assert resp["status"] == "error"

    @pytest.mark.negative
    def test_missing_action(self, client):
        resp = client.send_raw('{"id":"1","params":{}}')
        assert resp["status"] == "error"

    @pytest.mark.negative
    def test_missing_params(self, client):
        resp = client.send_raw('{"id":"1","action":"play_media"}')
        assert resp["status"] == "error"

    @pytest.mark.negative
    def test_wrong_field_type(self, client):
        resp = client.send_raw('{"id":"1","action":"set_volume","params":{"volume":"loud"}}')
        assert resp["status"] == "error"

    @pytest.mark.negative
    def test_volume_below_zero(self, client):
        resp = client.send_command("set_volume", {"volume": -1})
        assert resp["status"] == "error"

    @pytest.mark.negative
    def test_volume_above_100(self, client):
        resp = client.send_command("set_volume", {"volume": 101})
        assert resp["status"] == "error"

    @pytest.mark.negative
    def test_speed_above_240(self, client):
        resp = client.send_command("update_telemetry", {
            "speed": 241, "fuel": 50, "temperature": 90
        })
        assert resp["status"] == "error"

    @pytest.mark.negative
    def test_call_without_bluetooth(self, client):
        resp = client.send_command("incoming_call", {"caller": "+919999999999"})
        assert resp["status"] == "error"

    @pytest.mark.negative
    def test_pause_when_stopped(self, client):
        resp = client.send_command("pause_media")
        assert resp["status"] == "error"

    @pytest.mark.negative
    def test_invalid_state_no_mutation(self, client):
        """Invalid commands must not mutate state."""
        state_before = client.get_state()
        client.send_command("pause_media")  # Invalid - not playing
        state_after = client.get_state()
        assert state_before == state_after


# ============================================================
# State Transition Tests
# ============================================================

class TestStateTransitions:
    """Verify correct state machine transitions."""

    @pytest.mark.state_transition
    def test_media_state_machine(self, client):
        """Stopped → Playing → Paused → Playing → Stopped"""
        state = client.get_state()
        assert state["media_state"] == "stopped"

        client.send_command("play_media", {"track": "a.mp3"})
        assert client.get_state()["media_state"] == "playing"

        client.send_command("pause_media")
        assert client.get_state()["media_state"] == "paused"

        client.send_command("play_media", {"track": "a.mp3"})
        assert client.get_state()["media_state"] == "playing"

        client.send_command("stop_media")
        assert client.get_state()["media_state"] == "stopped"

    @pytest.mark.state_transition
    def test_bluetooth_state_machine(self, client):
        """Disconnected → Connected → Disconnected"""
        assert client.get_state()["bluetooth_state"] == "disconnected"

        client.send_command("connect_bluetooth", {"device": "Phone"})
        assert client.get_state()["bluetooth_state"] == "connected"

        client.send_command("disconnect_bluetooth")
        assert client.get_state()["bluetooth_state"] == "disconnected"

    @pytest.mark.state_transition
    def test_call_state_machine(self, client):
        """Idle → Ringing → Active → Idle"""
        client.send_command("connect_bluetooth", {"device": "Phone"})

        client.send_command("incoming_call", {"caller": "+1234567890"})
        assert client.get_state()["call_state"] == "ringing"

        client.send_command("accept_call")
        assert client.get_state()["call_state"] == "active"

        client.send_command("end_call")
        assert client.get_state()["call_state"] == "idle"


# ============================================================
# YAML Scenario Tests
# ============================================================

def get_scenario_files():
    """Get all non-suite YAML scenario files."""
    files = []
    if os.path.isdir(SCENARIO_DIR):
        for f in sorted(os.listdir(SCENARIO_DIR)):
            if f.endswith(".yaml") and "suite" not in f:
                files.append(os.path.join(SCENARIO_DIR, f))
    return files


@pytest.mark.regression
@pytest.mark.parametrize("scenario_file", get_scenario_files(),
                         ids=[os.path.basename(f) for f in get_scenario_files()])
def test_yaml_scenario(client, scenario_file):
    """Run a YAML-defined test scenario."""
    scenario = load_scenario(scenario_file)

    # Preconditions
    if scenario.preconditions.get("reset_state"):
        client.reset_state()

    # Execute steps
    for i, step in enumerate(scenario.steps):
        resp = client.send_command(step.action, step.params)

        if step.expect:
            errors = validate_response(resp, step.expect)
            assert not errors, (
                f"Step {i + 1} ({step.action}) failed: {errors}"
            )

    # Validate final state
    if scenario.final_expect and "state" in scenario.final_expect:
        final_state = client.get_state()
        errors = validate_state(final_state, scenario.final_expect["state"])
        assert not errors, f"Final state validation failed: {errors}"
