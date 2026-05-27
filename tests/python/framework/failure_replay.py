#!/usr/bin/env python3
"""Replay failed test scenarios from failure report files."""

import argparse
import json
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from framework.app_client import AppClient


def replay_failure(failure_file: str, host: str = "127.0.0.1", port: int = 5555) -> None:
    """Replay a failed scenario from its failure file."""
    with open(failure_file, "r") as f:
        failure_data = json.load(f)

    print(f"\nReplaying failed scenario: {failure_data['test_name']}")
    print(f"Description: {failure_data['description']}")
    print(f"Original failure time: {failure_data['timestamp']}")
    print(f"Original errors:")
    for err in failure_data.get("errors", []):
        print(f"  - {err}")
    print()

    client = AppClient(host, port)
    try:
        client.connect()

        # Reset state first
        if failure_data.get("preconditions", {}).get("reset_state", False):
            print("Resetting state...")
            client.reset_state()

        # Replay each step
        steps = failure_data.get("steps", [])
        for step in steps:
            action = step.get("action", "")
            params = step.get("params", {})

            print(f"\n  Step {step.get('step', '?')}: {action}")
            print(f"    Params: {json.dumps(params)}")

            try:
                response = client.send_command(action, params)
                status = response.get("status", "unknown")
                latency = response.get("_latency_ms", 0)

                print(f"    Status: {status} ({latency:.1f}ms)")

                if response.get("state"):
                    state = response["state"]
                    # Show key state fields
                    key_fields = ["media_state", "bluetooth_state", "call_state",
                                  "navigation_state", "active_screen"]
                    state_summary = {k: state[k] for k in key_fields if k in state}
                    print(f"    State: {json.dumps(state_summary)}")

                # Check if this step had errors originally
                if step.get("errors"):
                    print(f"    Original errors at this step:")
                    for err in step["errors"]:
                        print(f"      - {err}")

            except Exception as e:
                print(f"    ERROR: {e}")

        # Get final state
        print(f"\n  Final state:")
        final_state = client.get_state()
        print(f"    {json.dumps(final_state, indent=2)}")

    except Exception as e:
        print(f"Connection error: {e}")
    finally:
        client.disconnect()

    print(f"\nReplay complete.")


def main():
    parser = argparse.ArgumentParser(description="Replay failed test scenarios")
    parser.add_argument("--failure", required=True, help="Path to failure JSON file")
    parser.add_argument("--host", default="127.0.0.1", help="Server host")
    parser.add_argument("--port", type=int, default=5555, help="Server port")
    args = parser.parse_args()

    if not os.path.exists(args.failure):
        print(f"Failure file not found: {args.failure}")
        sys.exit(1)

    replay_failure(args.failure, args.host, args.port)


if __name__ == "__main__":
    main()
