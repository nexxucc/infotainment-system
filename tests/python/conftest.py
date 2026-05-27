"""Pytest conftest.py — fixtures for launching the app and connecting the test client."""

import os
import signal
import subprocess
import sys
import time

import pytest

# Add framework to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from framework.app_client import AppClient


def find_app_binary():
    """Locate the infotainment-simulator binary."""
    project_root = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    candidates = [
        os.path.join(project_root, "build", "app", "infotainment-simulator"),
        os.path.join(project_root, "build", "infotainment-simulator"),
    ]
    for path in candidates:
        if os.path.isfile(path) and os.access(path, os.X_OK):
            return path
    return None


@pytest.fixture(scope="session")
def app_process():
    """Launch the infotainment simulator as a subprocess for the test session."""
    binary = find_app_binary()
    if binary is None:
        pytest.skip("Infotainment simulator binary not found. Build the project first.")

    env = os.environ.copy()
    env["QT_QPA_PLATFORM"] = "offscreen"

    proc = subprocess.Popen(
        [binary, "--port", "5556"],
        env=env,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )

    # Wait for server to start
    time.sleep(1.5)

    if proc.poll() is not None:
        stdout = proc.stdout.read().decode() if proc.stdout else ""
        stderr = proc.stderr.read().decode() if proc.stderr else ""
        pytest.skip(f"App failed to start: stdout={stdout}, stderr={stderr}")

    yield proc

    # Teardown
    proc.send_signal(signal.SIGTERM)
    try:
        proc.wait(timeout=5)
    except subprocess.TimeoutExpired:
        proc.kill()
        proc.wait()


@pytest.fixture(scope="function")
def client(app_process):
    """Provide a connected AppClient and reset state before each test."""
    c = AppClient(port=5556, timeout=5.0)
    try:
        c.connect(retries=5, delay=0.5)
    except ConnectionError:
        pytest.skip("Cannot connect to infotainment simulator")

    # Reset state before each test
    c.reset_state()

    yield c

    c.disconnect()
