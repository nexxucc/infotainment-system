#!/usr/bin/env python3
"""Run YAML test scenarios against the infotainment simulator."""

import argparse
import json
import os
import sys
import time
from typing import List, Optional

# Add parent directory to path
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from framework.app_client import AppClient
from framework.scenario_loader import Scenario, load_scenario, load_suite, load_all_scenarios
from framework.validators import validate_response, validate_state
from framework.report_generator import ReportGenerator


class ScenarioResult:
    """Result of running a single scenario."""

    def __init__(self, scenario: Scenario):
        self.scenario = scenario
        self.passed = True
        self.errors: List[str] = []
        self.step_results: List[dict] = []
        self.latencies: List[float] = []
        self.start_time: float = 0
        self.end_time: float = 0

    @property
    def duration_ms(self) -> float:
        return (self.end_time - self.start_time) * 1000


class ScenarioRunner:
    """Execute test scenarios against the running infotainment simulator."""

    def __init__(self, host: str = "127.0.0.1", port: int = 5555):
        self.client = AppClient(host, port)
        self.results: List[ScenarioResult] = []

    def run_scenario(self, scenario: Scenario) -> ScenarioResult:
        """Run a single scenario."""
        result = ScenarioResult(scenario)
        result.start_time = time.time()

        try:
            self.client.connect()

            # Handle preconditions
            if scenario.preconditions.get("reset_state", False):
                self.client.reset_state()

            # Execute steps
            for i, step in enumerate(scenario.steps):
                step_result = {"step": i + 1, "action": step.action, "params": step.params}

                try:
                    response = self.client.send_command(step.action, step.params)
                    step_result["response"] = response
                    result.latencies.append(response.get("_latency_ms", 0))

                    # Validate step expectations
                    if step.expect:
                        errors = validate_response(response, step.expect)
                        if errors:
                            step_result["errors"] = errors
                            result.errors.extend(
                                [f"Step {i + 1} ({step.action}): {e}" for e in errors]
                            )
                            result.passed = False

                except Exception as e:
                    step_result["error"] = str(e)
                    result.errors.append(f"Step {i + 1} ({step.action}): {e}")
                    result.passed = False

                result.step_results.append(step_result)

            # Validate final state
            if scenario.final_expect and "state" in scenario.final_expect:
                try:
                    final_state = self.client.get_state()
                    errors = validate_state(final_state, scenario.final_expect["state"])
                    if errors:
                        result.errors.extend([f"Final state: {e}" for e in errors])
                        result.passed = False
                except Exception as e:
                    result.errors.append(f"Final state validation: {e}")
                    result.passed = False

        except Exception as e:
            result.errors.append(f"Connection error: {e}")
            result.passed = False
        finally:
            self.client.disconnect()

        result.end_time = time.time()
        self.results.append(result)
        return result

    def run_scenarios(self, scenarios: List[Scenario]) -> List[ScenarioResult]:
        """Run multiple scenarios."""
        results = []
        for scenario in scenarios:
            print(f"  Running: {scenario.test_name}...", end=" ", flush=True)
            result = self.run_scenario(scenario)
            status = "PASS ✓" if result.passed else "FAIL ✗"
            print(f"{status} ({result.duration_ms:.0f}ms)")
            if not result.passed:
                for error in result.errors[:3]:
                    print(f"    → {error}")
            results.append(result)
        return results

    def get_summary(self) -> dict:
        """Get summary of all results."""
        total = len(self.results)
        passed = sum(1 for r in self.results if r.passed)
        failed = total - passed
        all_latencies = []
        for r in self.results:
            all_latencies.extend(r.latencies)

        return {
            "total": total,
            "passed": passed,
            "failed": failed,
            "skipped": 0,
            "avg_latency_ms": sum(all_latencies) / len(all_latencies) if all_latencies else 0,
            "max_latency_ms": max(all_latencies) if all_latencies else 0,
            "failed_scenarios": [r.scenario.test_name for r in self.results if not r.passed],
        }


def main():
    parser = argparse.ArgumentParser(description="Run infotainment test scenarios")
    parser.add_argument("--scenario", help="Path to a single scenario YAML file")
    parser.add_argument("--suite", help="Name of suite to run (e.g., 'regression')")
    parser.add_argument("--dir", help="Directory containing scenario files")
    parser.add_argument("--host", default="127.0.0.1", help="Server host")
    parser.add_argument("--port", type=int, default=5555, help="Server port")
    parser.add_argument("--report", default="reports", help="Report output directory")
    args = parser.parse_args()

    runner = ScenarioRunner(args.host, args.port)

    scenarios = []
    scenario_dir = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
                                 "scenarios")

    if args.scenario:
        scenarios = [load_scenario(args.scenario)]
    elif args.suite:
        suite_file = os.path.join(scenario_dir, f"{args.suite}_suite.yaml")
        if os.path.exists(suite_file):
            scenarios = load_suite(suite_file)
        else:
            print(f"Suite file not found: {suite_file}")
            sys.exit(1)
    elif args.dir:
        scenarios = load_all_scenarios(args.dir)
    else:
        scenarios = load_all_scenarios(scenario_dir)

    if not scenarios:
        print("No scenarios found.")
        sys.exit(1)

    print(f"\nRunning {len(scenarios)} scenario(s)...\n")
    runner.run_scenarios(scenarios)

    summary = runner.get_summary()
    print(f"\n{'=' * 50}")
    print(f"Results: {summary['passed']}/{summary['total']} passed, "
          f"{summary['failed']} failed")
    if summary['avg_latency_ms'] > 0:
        print(f"Avg latency: {summary['avg_latency_ms']:.1f}ms, "
              f"Max: {summary['max_latency_ms']:.1f}ms")
    print(f"{'=' * 50}")

    # Generate reports
    report_gen = ReportGenerator(args.report)
    report_gen.generate(runner.results, summary)

    sys.exit(0 if summary["failed"] == 0 else 1)


if __name__ == "__main__":
    main()
