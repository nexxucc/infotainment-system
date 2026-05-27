"""Generate JSON and Markdown test reports."""

import json
import os
import time
from datetime import datetime, timezone
from typing import List


class ReportGenerator:
    """Generate test reports in JSON and Markdown formats."""

    def __init__(self, output_dir: str = "reports"):
        self.output_dir = output_dir
        os.makedirs(output_dir, exist_ok=True)
        os.makedirs(os.path.join(output_dir, "failures"), exist_ok=True)

    def generate(self, results: list, summary: dict) -> None:
        """Generate both JSON and Markdown reports."""
        report_data = self._build_report(results, summary)
        self._write_json_report(report_data)
        self._write_markdown_report(report_data)
        self._write_failure_files(results)

    def _build_report(self, results: list, summary: dict) -> dict:
        """Build the report data structure."""
        test_results = []
        for r in results:
            test_results.append({
                "test_name": r.scenario.test_name,
                "description": r.scenario.description,
                "passed": r.passed,
                "duration_ms": r.duration_ms,
                "errors": r.errors,
                "step_count": len(r.step_results),
                "avg_latency_ms": (
                    sum(r.latencies) / len(r.latencies) if r.latencies else 0
                ),
            })

        # Categorize failures
        failure_categories = {}
        for r in results:
            if not r.passed:
                for error in r.errors:
                    category = self._categorize_error(error)
                    failure_categories[category] = failure_categories.get(category, 0) + 1

        slowest = None
        for r in results:
            for step_r in r.step_results:
                resp = step_r.get("response", {})
                lat = resp.get("_latency_ms", 0)
                if slowest is None or lat > slowest.get("latency_ms", 0):
                    slowest = {
                        "scenario": r.scenario.test_name,
                        "action": step_r.get("action", ""),
                        "latency_ms": lat,
                    }

        return {
            "timestamp": datetime.now(timezone.utc).isoformat(),
            "total_tests": summary["total"],
            "passed": summary["passed"],
            "failed": summary["failed"],
            "skipped": summary["skipped"],
            "avg_response_latency_ms": summary["avg_latency_ms"],
            "slowest_command": slowest,
            "failure_category_counts": failure_categories,
            "failed_scenario_names": summary["failed_scenarios"],
            "test_results": test_results,
        }

    def _write_json_report(self, report_data: dict) -> None:
        """Write JSON report."""
        filepath = os.path.join(self.output_dir, "latest_report.json")
        with open(filepath, "w") as f:
            json.dump(report_data, f, indent=2)
        print(f"  JSON report: {filepath}")

    def _write_markdown_report(self, report_data: dict) -> None:
        """Write Markdown report."""
        filepath = os.path.join(self.output_dir, "latest_report.md")

        lines = [
            "# Test Report",
            f"\n**Generated**: {report_data['timestamp']}",
            "",
            "## Summary",
            "",
            f"| Metric | Value |",
            f"|--------|-------|",
            f"| Total Tests | {report_data['total_tests']} |",
            f"| Passed | {report_data['passed']} |",
            f"| Failed | {report_data['failed']} |",
            f"| Skipped | {report_data['skipped']} |",
            f"| Avg Latency | {report_data['avg_response_latency_ms']:.1f}ms |",
        ]

        if report_data.get("slowest_command"):
            sc = report_data["slowest_command"]
            lines.append(
                f"| Slowest Command | {sc['action']} ({sc['latency_ms']:.1f}ms) |"
            )

        if report_data["failed_scenario_names"]:
            lines.extend([
                "",
                "## Failed Scenarios",
                "",
            ])
            for name in report_data["failed_scenario_names"]:
                lines.append(f"- ❌ {name}")

        if report_data["failure_category_counts"]:
            lines.extend([
                "",
                "## Failure Categories",
                "",
                "| Category | Count |",
                "|----------|-------|",
            ])
            for cat, count in report_data["failure_category_counts"].items():
                lines.append(f"| {cat} | {count} |")

        lines.extend([
            "",
            "## Test Details",
            "",
        ])
        for tr in report_data["test_results"]:
            status = "✅" if tr["passed"] else "❌"
            lines.append(f"### {status} {tr['test_name']}")
            lines.append(f"\n{tr['description']}")
            lines.append(f"\n- Duration: {tr['duration_ms']:.0f}ms")
            lines.append(f"- Steps: {tr['step_count']}")
            if tr["errors"]:
                lines.append("- Errors:")
                for e in tr["errors"]:
                    lines.append(f"  - {e}")
            lines.append("")

        with open(filepath, "w") as f:
            f.write("\n".join(lines))
        print(f"  Markdown report: {filepath}")

    def _write_failure_files(self, results: list) -> None:
        """Write individual failure replay files."""
        for r in results:
            if not r.passed:
                failure_data = {
                    "test_name": r.scenario.test_name,
                    "description": r.scenario.description,
                    "errors": r.errors,
                    "steps": r.step_results,
                    "preconditions": r.scenario.preconditions,
                    "timestamp": datetime.now(timezone.utc).isoformat(),
                }
                filename = f"{r.scenario.test_name}.json"
                filepath = os.path.join(self.output_dir, "failures", filename)
                with open(filepath, "w") as f:
                    json.dump(failure_data, f, indent=2)

    @staticmethod
    def _categorize_error(error: str) -> str:
        """Categorize an error message."""
        error_lower = error.lower()
        if "connection" in error_lower:
            return "connection_error"
        if "timeout" in error_lower:
            return "timeout"
        if "state" in error_lower:
            return "state_validation"
        if "status" in error_lower:
            return "status_mismatch"
        if "missing" in error_lower:
            return "missing_field"
        return "other"
