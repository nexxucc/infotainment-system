"""Analyze structured JSON Lines log files from the infotainment simulator."""

import json
import os
from typing import List, Optional
from collections import Counter


class LogAnalyzer:
    """Parse and analyze JSONL log files."""

    def __init__(self, log_path: str = "logs/infotainment_events.jsonl"):
        self.log_path = log_path
        self.entries: List[dict] = []

    def load(self) -> None:
        """Load log entries from the JSONL file."""
        self.entries = []
        if not os.path.exists(self.log_path):
            return

        with open(self.log_path, "r") as f:
            for line in f:
                line = line.strip()
                if line:
                    try:
                        self.entries.append(json.loads(line))
                    except json.JSONDecodeError:
                        pass

    def filter_by_level(self, level: str) -> List[dict]:
        """Filter entries by log level."""
        return [e for e in self.entries if e.get("level") == level]

    def filter_by_component(self, component: str) -> List[dict]:
        """Filter entries by component name."""
        return [e for e in self.entries if e.get("component") == component]

    def filter_by_event_type(self, event_type: str) -> List[dict]:
        """Filter entries by event type."""
        return [e for e in self.entries if e.get("event_type") == event_type]

    def filter_by_correlation_id(self, correlation_id: str) -> List[dict]:
        """Get all entries for a specific correlation ID."""
        return [e for e in self.entries if e.get("correlation_id") == correlation_id]

    def get_error_entries(self) -> List[dict]:
        """Get all ERROR level entries."""
        return self.filter_by_level("ERROR")

    def get_event_type_counts(self) -> dict:
        """Count occurrences of each event type."""
        counter = Counter(e.get("event_type", "unknown") for e in self.entries)
        return dict(counter)

    def get_component_counts(self) -> dict:
        """Count log entries per component."""
        counter = Counter(e.get("component", "unknown") for e in self.entries)
        return dict(counter)

    def get_state_transitions(self, field: str) -> List[dict]:
        """Extract state transitions for a specific field from before/after states."""
        transitions = []
        for entry in self.entries:
            before = entry.get("before_state", {})
            after = entry.get("after_state", {})
            if isinstance(before, dict) and isinstance(after, dict):
                before_val = before.get(field)
                after_val = after.get(field)
                if before_val is not None and after_val is not None and before_val != after_val:
                    transitions.append({
                        "timestamp": entry.get("timestamp"),
                        "component": entry.get("component"),
                        "event_type": entry.get("event_type"),
                        "before": before_val,
                        "after": after_val,
                    })
        return transitions

    def get_summary(self) -> dict:
        """Get a summary of the log file."""
        return {
            "total_entries": len(self.entries),
            "by_level": {
                "INFO": len(self.filter_by_level("INFO")),
                "WARN": len(self.filter_by_level("WARN")),
                "ERROR": len(self.filter_by_level("ERROR")),
            },
            "by_component": self.get_component_counts(),
            "by_event_type": self.get_event_type_counts(),
            "error_messages": [
                e.get("message", "") for e in self.get_error_entries()
            ],
        }
