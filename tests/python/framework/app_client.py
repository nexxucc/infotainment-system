"""TCP client for communicating with the infotainment simulator's JSON command interface."""

import json
import socket
import time
import uuid
from typing import Optional


class AppClient:
    """Connects to the infotainment simulator's TCP JSON command server."""

    def __init__(self, host: str = "127.0.0.1", port: int = 5555, timeout: float = 5.0):
        self.host = host
        self.port = port
        self.timeout = timeout
        self.socket: Optional[socket.socket] = None
        self._buffer = b""

    def connect(self, retries: int = 5, delay: float = 1.0) -> None:
        """Connect to the command server with retries."""
        for attempt in range(retries):
            try:
                self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                self.socket.settimeout(self.timeout)
                self.socket.connect((self.host, self.port))
                self._buffer = b""
                return
            except (ConnectionRefusedError, OSError) as e:
                if attempt < retries - 1:
                    time.sleep(delay)
                else:
                    raise ConnectionError(
                        f"Failed to connect to {self.host}:{self.port} after {retries} attempts"
                    ) from e

    def disconnect(self) -> None:
        """Close the connection."""
        if self.socket:
            try:
                self.socket.close()
            except OSError:
                pass
            self.socket = None
            self._buffer = b""

    def send_command(self, action: str, params: Optional[dict] = None,
                     command_id: Optional[str] = None) -> dict:
        """Send a JSON command and receive the response."""
        if not self.socket:
            raise ConnectionError("Not connected to server")

        if command_id is None:
            command_id = str(uuid.uuid4())[:8]

        command = {
            "id": command_id,
            "action": action,
            "params": params or {}
        }

        start_time = time.time()
        data = json.dumps(command) + "\n"
        self.socket.sendall(data.encode("utf-8"))

        response = self._read_response()
        response["_latency_ms"] = (time.time() - start_time) * 1000
        return response

    def _read_response(self) -> dict:
        """Read a newline-delimited JSON response."""
        while True:
            if b"\n" in self._buffer:
                line, self._buffer = self._buffer.split(b"\n", 1)
                return json.loads(line.decode("utf-8"))

            try:
                chunk = self.socket.recv(4096)
            except socket.timeout:
                raise TimeoutError("Timed out waiting for response")

            if not chunk:
                raise ConnectionError("Server closed connection")

            self._buffer += chunk

    def send_raw(self, data: str) -> dict:
        """Send raw data (for testing malformed input)."""
        if not self.socket:
            raise ConnectionError("Not connected to server")

        self.socket.sendall((data + "\n").encode("utf-8"))
        return self._read_response()

    def get_state(self) -> dict:
        """Shortcut to get current application state."""
        resp = self.send_command("get_state")
        return resp.get("state", {})

    def reset_state(self) -> dict:
        """Shortcut to reset application state."""
        return self.send_command("reset_state")

    def __enter__(self):
        self.connect()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.disconnect()
