"""Validate responses and states from the infotainment simulator."""

from typing import List, Tuple


def validate_response(response: dict, expected: dict) -> List[str]:
    """Validate a command response against expected values.

    Returns a list of validation error messages (empty if valid).
    """
    errors = []

    if "status" in expected:
        if response.get("status") != expected["status"]:
            errors.append(
                f"Expected status '{expected['status']}', got '{response.get('status')}'"
            )

    if "error_code" in expected:
        if response.get("error_code") != expected["error_code"]:
            errors.append(
                f"Expected error_code '{expected['error_code']}', "
                f"got '{response.get('error_code')}'"
            )

    if "state" in expected:
        state = response.get("state", {})
        state_errors = validate_state(state, expected["state"])
        errors.extend(state_errors)

    return errors


def validate_state(actual_state: dict, expected_state: dict) -> List[str]:
    """Validate actual state against expected state fields.

    Only checks fields present in expected_state.
    """
    errors = []

    for key, expected_value in expected_state.items():
        if key not in actual_state:
            errors.append(f"Missing state field: '{key}'")
            continue

        actual_value = actual_state[key]
        if actual_value != expected_value:
            errors.append(
                f"State '{key}': expected '{expected_value}', got '{actual_value}'"
            )

    return errors


def validate_latency(response: dict, max_ms: float = 100.0) -> List[str]:
    """Validate that response latency is within acceptable limits."""
    errors = []
    latency = response.get("_latency_ms", 0)
    if latency > max_ms:
        errors.append(f"Response latency {latency:.1f}ms exceeds limit of {max_ms}ms")
    return errors
