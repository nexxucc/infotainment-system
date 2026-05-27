"""Load and validate YAML test scenarios."""

import os
import yaml
from typing import List, Optional


class ScenarioStep:
    """A single step in a test scenario."""

    def __init__(self, action: str, params: dict, expect: Optional[dict] = None):
        self.action = action
        self.params = params
        self.expect = expect or {}


class Scenario:
    """A complete test scenario loaded from YAML."""

    def __init__(self, test_name: str, description: str, steps: List[ScenarioStep],
                 preconditions: Optional[dict] = None, final_expect: Optional[dict] = None):
        self.test_name = test_name
        self.description = description
        self.steps = steps
        self.preconditions = preconditions or {}
        self.final_expect = final_expect or {}


def load_scenario(filepath: str) -> Scenario:
    """Load a single scenario from a YAML file."""
    with open(filepath, "r") as f:
        data = yaml.safe_load(f)

    return _parse_scenario(data)


def load_suite(filepath: str) -> List[Scenario]:
    """Load a suite file that references multiple scenarios."""
    with open(filepath, "r") as f:
        data = yaml.safe_load(f)

    if "scenarios" in data:
        # Suite file with list of scenario file paths
        base_dir = os.path.dirname(filepath)
        scenarios = []
        for ref in data["scenarios"]:
            path = ref if os.path.isabs(ref) else os.path.join(base_dir, ref)
            scenarios.append(load_scenario(path))
        return scenarios

    # Single scenario file
    return [_parse_scenario(data)]


def load_all_scenarios(directory: str) -> List[Scenario]:
    """Load all YAML scenarios from a directory."""
    scenarios = []
    for filename in sorted(os.listdir(directory)):
        if filename.endswith(".yaml") or filename.endswith(".yml"):
            filepath = os.path.join(directory, filename)
            try:
                if "regression_suite" in filename:
                    scenarios.extend(load_suite(filepath))
                else:
                    scenarios.append(load_scenario(filepath))
            except Exception as e:
                print(f"Warning: Failed to load {filepath}: {e}")
    return scenarios


def _parse_scenario(data: dict) -> Scenario:
    """Parse a scenario from a dictionary."""
    steps = []
    for step_data in data.get("steps", []):
        steps.append(ScenarioStep(
            action=step_data["action"],
            params=step_data.get("params", {}),
            expect=step_data.get("expect")
        ))

    return Scenario(
        test_name=data.get("test_name", "unnamed"),
        description=data.get("description", ""),
        steps=steps,
        preconditions=data.get("preconditions"),
        final_expect=data.get("final_expect")
    )
